#include "ffschedule.h"
#include "ffstorage.h"
#include "ffop.h"

#define INITIAL_FFSCHEDULE_POOL_COUNT 1024

static pool_h schedule_pool;

static uint64_t schedid = 0;

int ffschedule_init(){

    schedule_pool = ffstorage_pool_create(sizeof(ffschedule_t), INITIAL_FFSCHEDULE_POOL_COUNT);

    return FFSUCCESS;
}


int ffschedule_create(ffschedule_h * handle){
    ffschedule_t ** sched = (ffschedule_t **) handle; 
    ffstorage_pool_get(schedule_pool, (void **) sched);
    (*sched)->oplist = NULL;
    (*sched)->id = schedid++;   
    (*sched)->tmpmem = NULL;

    ffnop(0, (ffop_h *) &((*sched)->begin_op));
    ffnop(0, (ffop_h *) &((*sched)->end_op));

    return FFSUCCESS;
}

int ffschedule_delete(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 

    if (sched->tmpmem != NULL) free(sched->tmpmem);
    return ffstorage_pool_put(sched);
}

int ffschedule_add_op(ffschedule_h schedh, ffop_h oph){
    ffschedule_t * sched = (ffschedule_t *) schedh;
    ffop_t * op = (ffop_t *) oph;
    
    op->sched_next = sched->oplist;
    sched->oplist = op;

    FFLOG("adding op %lu to schedule %lu\n", op->id, sched->id);
    
    if (op->in_dep_count==0){
        FFLOG("making op %lu dependent from begin; schedule %lu\n", op->id, sched->id);
        ffop_hb((ffop_h) sched->begin_op, (ffop_h) op);
    }

    if (op->out_dep_count==0){
        FFLOG("making end_op dependent from op %lu; schedule: %lu\n", op->id, sched->id);
        ffop_hb((ffop_h) op, (ffop_h) sched->end_op);
    }

    return FFSUCCESS;
}

int ffschedule_post(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    FFLOG("Posting schedule %lu\n", sched->id);
    return ffop_post((ffop_h) sched->begin_op);
}

int ffschedule_wait(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    FFLOG("Waiting on schedule %lu (end_op: %lu)\n", sched->id, sched->end_op->id);
    return ffop_wait((ffop_h) sched->end_op);
}

int ffschedule_test(ffschedule_h handle, int * flag){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    return ffop_test((ffop_h) sched->end_op, flag);
}


int ffschedule_set_tmpmem(ffschedule_h handle, void * mem){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    if (sched==NULL || mem==NULL) return FFINVALID_ARG;
    sched->tmpmem = mem;
    return FFSUCCESS;
}

