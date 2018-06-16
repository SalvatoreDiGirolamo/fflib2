
#include "ffop.h"
#include "ffsend.h"
#include "ffrecv.h"
#include "ffstorage.h"
#include <sched.h>

#define INITIAL_FFOP_POOL_COUNT 1024

pool_h op_pool;

int ffop_init(){

    /* the MPI backend doesn't do anything in the init, so we use a common
       init function. */

    op_pool = ffstorage_pool_create(sizeof(ffop_t), INITIAL_FFOP_POOL_COUNT);

    return FFSUCCESS;
}


int ffop_finalize(){   
    return ffstorage_pool_destroy(op_pool);
}

int ffop_free(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;
    ffstorage_pool_put(op);
}

int ffop_post(ffop_h _op){
    int res;
    ffop_t * op = (ffop_t *) _op;
#ifdef ARGS_CHECK
    if (op->type<0 || op->type>FFMAX_IDX) return FFINVALID_ARG;
#endif
    op->instance.posted=1;
    res = ff.impl.ops[op->type].post(op, NULL);

    /* check if the operation has been immediately completed */
    if (op->instance.completed){ ffop_complete(op); }
    
    return res;
}

int ffop_wait(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;

    if (!op->instance.posted && op->in_dep_count==0){
        FFLOG_ERROR("Waiting on an independent op that has not been posted!");
        return FFINVALID_ARG;
    }

    uint32_t polls=0;
    while (op->instance.completed==0){
        if (polls >= FFPOLLS_BEFORE_YIELD){
            polls=0;
            sched_yield();
        }else {
            polls++;
        }
    }

    return FFSUCCESS;
}

int ffop_test(ffop_h _op, int * flag){
    ffop_t * op = (ffop_t *) _op;
    
    if (!op->instance.posted && op->in_dep_count==0){
        FFLOG_ERROR("Testing an independent op that has not been posted!");
        return FFINVALID_ARG;
    }

    return op->instance.completed!=0;
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

    first->dependent[idx] = second;
    __sync_fetch_and_add(&(second->in_dep_count), 1);
    second->instance.dep_left = second->in_dep_count;

    return FFSUCCESS;
}


/* internal (called by ffsend, ffrecv, others) */
int ffop_create(ffop_t ** ptr){
    ffstorage_pool_get(op_pool, (void **) ptr);
    
    ffop_t * op = *ptr;

    op->out_dep_count=0;
    op->in_dep_count=0;
    op->sched_next = NULL;

    op->instance.next = NULL;
    op->instance.completed = 0;
    op->instance.posted = 0;
    op->instance.dep_left = 0;

    return FFSUCCESS;
}

int ffop_complete(ffop_t * op){
    for (int i=0; i<op->out_dep_count; i++){
        ffop_t * dep_op = op->dependent[i];

        uint32_t deps = __sync_add_and_fetch(&(dep_op->instance.dep_left), -1);
        FFLOG("Decreasing %p dependencies by one: now %i\n", dep_op, dep_op->instance.dep_left);
        if (deps==0){
            FFLOG("All dependencies of %p are satisfied: posting it!\n", dep_op);
            ffop_post((ffop_h) dep_op);
        }
    }
    return FFSUCCESS;
}
