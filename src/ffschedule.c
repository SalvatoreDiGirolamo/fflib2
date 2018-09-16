#include "ffschedule.h"
#include "ffstorage.h"
#include "ffop.h"

#define INITIAL_FFSCHEDULE_POOL_COUNT 1024

static pool_h schedule_pool;

static uint64_t schedid = 0;

int ffschedule_init(){
    schedule_pool = ffstorage_pool_create(sizeof(ffschedule_t), INITIAL_FFSCHEDULE_POOL_COUNT, NULL);
    return FFSUCCESS;
}


int ffschedule_create(ffschedule_h * handle){
    ffschedule_t ** sched       = (ffschedule_t **) handle; 
    ffstorage_pool_get(schedule_pool, (void **) sched);

    (*sched)->oplist            = NULL;
    (*sched)->id                = schedid++;   
    (*sched)->state             = NULL;

    (*sched)->post_fun          = ffschedule_default_post;
    (*sched)->delete_fun        = ffschedule_default_delete;
    (*sched)->wait_fun          = ffschedule_default_wait;
    (*sched)->test_fun          = ffschedule_default_test;
    
    ffnop(0, (ffop_h *) &((*sched)->begin_op));
    ffnop(0, (ffop_h *) &((*sched)->end_op));

    return FFSUCCESS;
}


/* schedule actions */

int ffschedule_delete(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    return sched->delete_fun(handle);
}   

int ffschedule_post(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    return sched->post_fun(handle);
}

int ffschedule_wait(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    return sched->wait_fun(handle);
}

int ffschedule_test(ffschedule_h handle, int * flag){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    return sched->test_fun(handle, flag);
}


/* schedule default actions */

int ffschedule_default_delete(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    return ffstorage_pool_put(sched);
}

int ffschedule_default_post(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    FFLOG("Posting schedule %lu\n", sched->id);
    return ffop_post((ffop_h) sched->begin_op);
}

int ffschedule_default_wait(ffschedule_h handle){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    FFLOG("Waiting on schedule %lu (end_op: %lu)\n", sched->id, sched->end_op->id);
#ifndef FFPROGRESS_THREAD
    FFLOG_ERROR("Not implemented (FFPROGRESS_THREAD not defined)!\n");
    exit(1);
#endif

    FFLOG("sched->begin_op->version: %u; sched->end_op->version: %u; sched->end_op->completed: %u\n", sched->begin_op->version, sched->end_op->version, (uint32_t) sched->end_op->instance.completed);
    return ffop_wait((ffop_h) sched->end_op);
}

int ffschedule_default_test(ffschedule_h handle, int * flag){
    ffschedule_t * sched = (ffschedule_t *) handle; 
#ifndef FFPROGRESS_THREAD
    FFLOG_ERROR("Not implemented (FFPROGRESS_THREAD not defined)!\n");
    exit(1);
#endif
    return ffop_test((ffop_h) sched->end_op, flag);
}


/* function setters */

int ffschedule_set_post_fun(ffschedule_h handle, ffschedule_post_fun_t fun){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    sched->post_fun = fun;
    return FFSUCCESS;
}

int ffschedule_set_delete_fun(ffschedule_h handle, ffschedule_delete_fun_t fun){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    sched->delete_fun = fun;
    return FFSUCCESS;
}

int ffschedule_set_wait_fun(ffschedule_h handle, ffschedule_wait_fun_t fun){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    sched->wait_fun = fun;
    return FFSUCCESS;
}

int ffschedule_set_test_fun(ffschedule_h handle, ffschedule_test_fun_t fun){
    ffschedule_t * sched = (ffschedule_t *) handle; 
    sched->test_fun = fun;
    return FFSUCCESS;
}


/* others FIXME: adjust nicely */

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


int ffschedule_add_op(ffschedule_h schedh, ffop_h oph){
    ffschedule_t * sched = (ffschedule_t *) schedh;
    ffop_t * op = (ffop_t *) oph;
    
    op->sched_next = sched->oplist;
    sched->oplist = op;

    FFLOG("adding op %lu to schedule %lu\n", op->id, sched->id);
    
    if (op->in_dep_count==0){
        FFLOG("making op %lu dependent from begin; schedule %lu\n", op->id, sched->id);
        ffop_hb((ffop_h) sched->begin_op, (ffop_h) op, 0);
    }

    if (op->dep_next==NULL){
        FFLOG("making end_op dependent from op %lu; schedule: %lu\n", op->id, sched->id);
        ffop_hb((ffop_h) op, (ffop_h) sched->end_op, 0);
    }

    return FFSUCCESS;
}

int ffschedule_get_begin_op(ffschedule_h schedh, ffop_h *oph){
    ffschedule_t * sched = (ffschedule_t *) schedh;
    *oph = (ffop_h) sched->begin_op;   
    return FFSUCCESS;
}

int ffschedule_get_end_op(ffschedule_h schedh, ffop_h *oph){
    ffschedule_t * sched = (ffschedule_t *) schedh;
    *oph = (ffop_h) sched->end_op;   
    return FFSUCCESS;
}

