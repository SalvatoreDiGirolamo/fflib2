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
    ffschedule_t ** sched       = (ffschedule_t **) handle; 
    ffstorage_pool_get(schedule_pool, (void **) sched);

    (*sched)->oplist            = NULL;
    (*sched)->id                = schedid++;   
    (*sched)->state             = NULL;

    (*sched)->post_callback     = NULL;
    (*sched)->delete_callback   = NULL;

    ffnop(0, (ffop_h *) &((*sched)->begin_op));
    ffnop(0, (ffop_h *) &((*sched)->end_op));

    return FFSUCCESS;
}

int ffschedule_delete(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    if (sched->delete_callback!=NULL) sched->delete_callback(handle);
    return ffstorage_pool_put(sched);
}

int ffschedule_set_state(ffschedule_h handle, void * state){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    sched->state = state;
    return FFSUCCESS;
}

int ffschedule_get_state(ffschedule_h handle, void ** state){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    *state = sched->state;
    return FFSUCCESS;
}

int ffschedule_set_post_callback(ffschedule_h handle, ffschedule_post_callback_t cb){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    sched->post_callback = cb;
    return FFSUCCESS;
}

int ffschedule_set_delete_callback(ffschedule_h handle, ffschedule_delete_callback_t cb){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    sched->delete_callback = cb;
    return FFSUCCESS;
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
    if (sched->post_callback!=NULL) sched->post_callback(handle);
    return ffop_post((ffop_h) sched->begin_op);
}

int ffschedule_wait(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    FFLOG("Waiting on schedule %lu (end_op: %lu)\n", sched->id, sched->end_op->id);
#ifndef FFPROGRESS_THREAD
    FFLOG_ERROR("Not implemented (FFPROGRESS_THREAD not defined)!\n");
    exit(1);
#endif
    return ffop_wait((ffop_h) sched->end_op);
}

int ffschedule_test(ffschedule_h handle, int * flag){
    ffschedule_t * sched = (ffschedule_t *) handle; 
#ifndef FFPROGRESS_THREAD
    FFLOG_ERROR("Not implemented (FFPROGRESS_THREAD not defined)!\n");
    exit(1);
#endif
    return ffop_test((ffop_h) sched->end_op, flag);
}

