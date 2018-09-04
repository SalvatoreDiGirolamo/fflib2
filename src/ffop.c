
#include "ffop.h"
#include "ffsend.h"
#include "ffrecv.h"
#include "ffstorage.h"
#include <sched.h>

#define INITIAL_FFOP_POOL_COUNT 1024
#define INITIAL_FFDEP_OP_POOL_COUNT 1024

static pool_h op_pool;
static pool_h dep_op_pool;

static uint64_t opid = 0;

int ffop_init(){

    op_pool = ffstorage_pool_create(sizeof(ffop_t), INITIAL_FFOP_POOL_COUNT);
    dep_op_pool = ffstorage_pool_create(sizeof(ffdep_op_t), INITIAL_FFDEP_OP_POOL_COUNT);

    return FFSUCCESS;
}


int ffop_finalize(){   
    ffstorage_pool_destroy(op_pool);
    ffstorage_pool_destroy(dep_op_pool);
    return FFSUCCESS;
}

int ffop_free(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;
    //FIXME one should create/destroy the mutex/cond only when
    //the op is created for the first time and freed for the last time
    //(i.e., by the pool).
#ifdef WAIT_COND
    FFMUTEX_DESTROY(op->mutex);
    FFCOND_DESTROY(op->cond);
#endif
    return ffstorage_pool_put(op);
}

int ffop_tostring(ffop_h _op, char * str, int len){
    ffop_t * op = (ffop_t *) _op;
    ff.impl.ops[op->type].tostring(op, str, len);
    return FFSUCCESS;
}

int ffop_post(ffop_h _op){
    int res;
    ffop_t * op = (ffop_t *) _op;

#ifdef FFDEBUG
    if (op->instance.dep_left>0) FFLOG("Posting an op with dependencies left!\n");
#endif

#ifdef ARGS_CHECK
    if (op->type<0 || op->type>FFMAX_IDX) {
        FFLOG("Invalid arg: op->type: %u\n", op->type);
        return FFINVALID_ARG;
    }
#endif

    if (op->version>0 && IS_OPT_SET(op, FFOP_NON_PERSISTENT)){
        FFLOG("Re-posting a non-persistent operation is not allowed!\n");
        return FFINVALID_ARG;
    }

    if (op->in_flight){
        FFLOG("Cancelling op %lu (version: %u)\n", op->id, op->version);
        ffop_cancel(_op);
    }
    op->in_flight = 1;

    op->instance.completed = 0;

    FFLOG("Posting op %lu\n", op->id);
    //__sync_fetch_and_add(&(op->instance.posted_version), 1);
    res = ff.impl.ops[op->type].post(op, NULL);

    __sync_fetch_and_add(&(op->version), 1);

    /* check if the operation has been immediately completed */
    if (res==FFCOMPLETED){ ffop_complete(op); }
    
    return res;
}

int ffop_wait(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;

    FFLOG("Waiting op %lu (version: %u; wait_version: %u; waiting for: %u)\n", op->id, op->version, op->wait_version, op->wait_version+1);

#ifdef FFPROGRESS_THREAD
    uint32_t polls=0;

#ifdef WAIT_COND
    while (!FFOP_TEST_VERSION_COMPLETION(op, op->wait_version+1)){
        FFCOND_WAIT(op->cond, op->mutex);
    }
#else
    while (!FFOP_TEST_VERSION_COMPLETION(op, op->wait_version+1)){
        if (polls >= FFPOLLS_BEFORE_YIELD){
            polls=0;
            sched_yield();
        }else {
            polls++;
        }
    }
#endif
    op->instance.completed = 0;
    op->wait_version++;


    //FFLOG("Wait on %p finished: version: %u; posted version: %u; completed version: %u\n", op, op->version, op->instance.posted_version, op->instance.completed_version);
    return FFSUCCESS;
#else
    ff.impl.ops[op->type].wait(op); //FIXME: check this return value
    return ffop_complete(op);
#endif
}

int ffop_test(ffop_h _op, int * flag){
    ffop_t * op = (ffop_t *) _op;
    
#ifdef FFPROGRESS_THREAD
    *flag = FFOP_TEST_VERSION_COMPLETION(op, op->wait_version+1);

    if (*flag){
        op->instance.completed=0;
        op->wait_version++;
    }
    return FFSUCCESS;
#else
    FFLOG_ERROR("This path is not tested!!!\n");
    ff.impl.ops[op->type].test(op, flag); //FIXME: check this return value
    if (*flag){
        op->instance.completed=0;
        op->wait_version++;
        return ffop_complete(op);

    }
    return FFSUCCESS;
#endif
}


int ffop_hb(ffop_h _first, ffop_h _second, int options){
    ffop_t * first = (ffop_t *) _first;
    ffop_t * second = (ffop_t *) _second;
#ifdef ARGS_CHECK
    if (first==NULL || second==NULL) return FFINVALID_ARG;
#endif

    FFLOG("HB: %lu -> %lu\n", first->id, second->id);

    FFGRAPH(_first, _second);
    
    ffdep_op_t *dep;        
    ffstorage_pool_get(dep_op_pool, (void **) &dep);
    dep->op = second;
    dep->options = options;

    if (first->dep_first==NULL){
        first->dep_first        = dep;
        first->dep_last         = dep;
        first->dep_next         = dep;
        dep->next               = dep;
    }else{
        dep->next               = first->dep_first;
        first->dep_last->next   = dep;
        first->dep_last         = dep;
    }

    __sync_fetch_and_add(&(second->in_dep_count), 1);
    second->instance.dep_left = second->in_dep_count;

    return FFSUCCESS;
}


/* internal (called by ffsend, ffrecv, others) */
int ffop_create(ffop_t ** ptr){
    ffstorage_pool_get(op_pool, (void **) ptr);
    
    ffop_t * op                     = *ptr;

    op->id                          = opid++;
    op->in_dep_count                = 0;
    op->sched_next                  = NULL;
    op->options                     = FFOP_DEP_AND;
    op->version                     = 0;
    op->wait_version                = 0;

    op->dep_next                    = NULL;
    op->dep_first                   = NULL;
    op->dep_last                    = NULL;
    op->in_flight                   = 0;

    op->instance.next               = NULL;
    op->instance.dep_left           = 0;
    //op->instance.posted_version     = 0;
    //op->instance.completed_version  = 0;
    op->instance.completed          = 0;

#ifdef WAIT_COND
    FFMUTEX_INIT(op->mutex);
    FFCOND_INIT(op->cond);
#endif

    return FFSUCCESS;
}

int ffop_complete(ffop_t * op){

    FFLOG("completing op %lu\n", op->id);
    op->in_flight = 0;
    // restore dep_left, so the op can be reused
    op->instance.dep_left = op->in_dep_count; 
    __sync_add_and_fetch(&(op->instance.completed), 1);

#ifdef WAIT_COND
    FFCOND_SIGNAL(op->cond, op->mutex);
#endif

    if (op->dep_next==NULL) return FFSUCCESS;
    ffdep_op_t * first_dep = op->dep_next;
    
    uint8_t satisfy_all = !IS_OPT_SET(op, FFOP_DEP_FIRST);

    do{
        ffdep_op_t * dep = op->dep_next;
        ffop_t * dep_op = dep->op;
        op->dep_next = op->dep_next->next;

        if (op->version <= dep_op->version && !IS_OPT_SET(dep, FFDEP_IGNORE_VERSION)) {
            FFLOG("ffop version mismatch -> dependency not satisfied (%lu.version (dep_op) = %u; %lu.version (op) = %u)\n", dep_op->id, dep_op->version, op->id, op->version);
            continue;
        }

        if (IS_OPT_SET(dep_op, FFOP_NO_AUTOPOST)){
            FFLOG("op %lu is set with FFOP_NO_AUTOPOST --> skipping it!\n", dep_op->id);
            continue;
        }

        uint32_t deps = __sync_add_and_fetch(&(dep_op->instance.dep_left), -1);
        FFLOG("Decreasing %lu dependencies by one: now %i (is OR dep: %u; non-persistent: %u); %lu.version (dep_op) = %u; %lu.version (op) = %u\n", dep_op->id, dep_op->instance.dep_left, (unsigned int) IS_OPT_SET(dep_op, FFOP_DEP_OR), (unsigned int) IS_OPT_SET(dep_op, FFOP_NON_PERSISTENT), dep_op->id, dep_op->version, op->id, op->version);

        int trigger;
        // triggering conditions:
        // no dependencies left if is an AND dependency *or* at least one is satisfied if it is an OR dep
        trigger  = deps <= 0 || IS_OPT_SET(dep_op, FFOP_DEP_OR); 
        // the op has not been already posted *or* the op is persistent
        trigger &= (dep_op->version==0 || !IS_OPT_SET(dep_op, FFOP_NON_PERSISTENT));
    
        if (trigger){
            FFLOG("All dependencies of %lu are satisfied: posting it!\n", dep_op->id);
            ffop_post((ffop_h) dep_op);
        }
        
    } while (op->dep_next != first_dep && satisfy_all);

    return FFSUCCESS;
}

int ffop_cancel(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;
    return ff.impl.ops[op->type].cancel(op); 
}
