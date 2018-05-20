
#include "ffop.h"
#include "ffsend.h"
#include "ffrecv.h"

#include "mpi/ffop_mpi.h"

#include <sched.h>

ffop_descriptor_t ops[FFMAX_IDX];

int ffop_init(){

    ops[FFSEND].init = ffop_mpi_init;
    ops[FFSEND].post = ffop_mpi_send_post;

    ops[FFRECV].init = ffop_mpi_init;
    ops[FFRECV].post = ffop_mpi_recv_post;

    return FFSUCCESS;
}

int ffop_post(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;
#ifdef ARGS_CHECK
    if (ops->type<0 || ops->type>FFMAX_IDX) return FFINVALID_ARG;
#endif
    return ops[op->type].post(op, NULL);
}

int ffop_wait(ffop_h _op){
    ffop_t * op = (ffop_t *) _op;

    uint32_t polls=0;
    while (op->completed==0){
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
    return op->completed!=0;
}


int ffop_happens_before(ffop_h _first, ffop_h _second){
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

    return FFSUCCESS;
}



