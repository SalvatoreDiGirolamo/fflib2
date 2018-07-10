
#include "ffop.h"
#include "ffsend.h"
#include "ffrecv.h"
#include "ffstorage.h"
#include <sched.h>

#define INITIAL_FFOP_POOL_COUNT 1024

static pool_h op_pool;

static uint64_t opid = 0;

int ffop_init(){

    op_pool = ffstorage_pool_create(sizeof(ffop_t), INITIAL_FFOP_POOL_COUNT);

    return FFSUCCESS;
}


int ffop_finalize(){   
    return ffstorage_pool_destroy(op_pool);
}

int ffop_free(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;
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

    op->instance.completed = 0;

    FFLOG("Posting op %lu\n", op->id);
    //__sync_fetch_and_add(&(op->instance.posted_version), 1);
    res = ff.impl.ops[op->type].post(op, NULL);

    /* check if the operation has been immediately completed */
    if (res==FFCOMPLETED){ ffop_complete(op); }
    
    return res;
}

int ffop_wait(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;

    uint32_t polls=0;
    while (!FFOP_IS_COMPLETED(op)){
        if (polls >= FFPOLLS_BEFORE_YIELD){
            polls=0;
            sched_yield();
        }else {
            polls++;
        }
    }

    op->instance.completed = 0;
    //FFLOG("Wait on %p finished: version: %u; posted version: %u; completed version: %u\n", op, op->version, op->instance.posted_version, op->instance.completed_version);
    return FFSUCCESS;
}

int ffop_test(ffop_h _op, int * flag){
    ffop_t * op = (ffop_t *) _op;

    *flag = FFOP_IS_COMPLETED(op);

    if (*flag){
        op->instance.completed=0;
    }
    return FFSUCCESS;

}


int ffop_hb(ffop_h _first, ffop_h _second){
    ffop_t * first = (ffop_t *) _first;
    ffop_t * second = (ffop_t *) _second;
#ifdef ARGS_CHECK
    if (first==NULL || second==NULL) return FFINVALID_ARG;
#endif

    uint32_t idx = __sync_fetch_and_add(&(first->out_dep_count), 1);

#ifdef ARGS_CHECK
    if (idx > MAX_DEPS) return FFTOO_MANY_DEPS;
#endif

    FFLOG("HB: %lu -> %lu\n", first->id, second->id);

    FFGRAPH(_first, _second);

    first->dependent[idx] = second;
    __sync_fetch_and_add(&(second->in_dep_count), 1);
    second->instance.dep_left = second->in_dep_count;

    return FFSUCCESS;
}


/* internal (called by ffsend, ffrecv, others) */
int ffop_create(ffop_t ** ptr){
    ffstorage_pool_get(op_pool, (void **) ptr);
    
    ffop_t * op                     = *ptr;

    op->id                          = opid++;
    op->out_dep_count               = 0;
    op->in_dep_count                = 0;
    op->sched_next                  = NULL;
    op->options                     = FFOP_DEP_AND;
    op->version                     = 0;

    op->instance.next               = NULL;
    op->instance.dep_left           = 0;
    //op->instance.posted_version     = 0;
    //op->instance.completed_version  = 0;
    op->instance.completed          = 0;

    return FFSUCCESS;
}

int ffop_complete(ffop_t * op){

    FFLOG("completing op %lu\n", op->id);

    // increment version && restore dep_left, so the op can be reused
    __sync_fetch_and_add(&(op->version), 1);
    op->instance.dep_left = op->in_dep_count; 
    __sync_add_and_fetch(&(op->instance.completed), 1);

    for (int i=0; i<op->out_dep_count; i++){
        ffop_t * dep_op = op->dependent[i];

        uint32_t deps = __sync_add_and_fetch(&(dep_op->instance.dep_left), -1);
        FFLOG("Decreasing %lu dependencies by one: now %i\n", dep_op->id, dep_op->instance.dep_left);

        int trigger;
        // triggering conditions:
        // no dependencies left if is an AND dependency OR at least one satisfied if is an OR dep
        trigger  = deps == 0 || IS_OPT_SET(op, FFOP_DEP_OR); 
        // the op has not been posted already OR the op is persistent
        trigger &= (op->version==0 || !IS_OPT_SET(op, FFOP_NON_PERSISTENT));

        if (trigger){
            FFLOG("All dependencies of %lu are satisfied: posting it!\n", dep_op->id);
            ffop_post((ffop_h) dep_op);
        }

    }
    return FFSUCCESS;
}
