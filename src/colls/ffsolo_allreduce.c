#include "ffcollectives.h"

typedef struct solo_allreduce_state{
    ffschedule_h activation_schedule;
    ffschedule_h allreduce_schedule;
    ffschedule_h solo_limiter;
    ffop_h allreduce_activation_test;  //op that needs to be tested to check if a local activation completed (two activations should not overlap)
} solo_allreduce_state_t;

int ffsolo_allreduce_post(ffschedule_h sched);
int ffsolo_allreduce_wait(ffschedule_h sched);
int ffsolo_allreduce_test(ffschedule_h sched, int * flag);
int ffsolo_allreduce_delete(ffschedule_h sched);
int ffsolo_allreduce_start(ffschedule_h sched);
int ffsolo_allreduce_print(ffschedule_h handle, FILE * fp, char * name);

int ffsolo_allreduce(void * sndbuff, void * rcvbuff, int count, int16_t tag, ffoperator_h operator, ffdatatype_h datatype, int options, int async, ffschedule_h * _sched){

    solo_allreduce_state_t * state = (solo_allreduce_state_t *) malloc(sizeof(solo_allreduce_state_t));

    ffschedule_h sched;
    FFCALL(ffschedule_create(&sched));
    ffschedule_set_state(sched, state);
 
    ffschedule_set_start_fun(sched, ffsolo_allreduce_start);
    ffschedule_set_post_fun(sched, ffsolo_allreduce_post);
    ffschedule_set_wait_fun(sched, ffsolo_allreduce_wait);
    ffschedule_set_test_fun(sched, ffsolo_allreduce_test);
    ffschedule_set_delete_fun(sched, ffsolo_allreduce_delete);
    ffschedule_set_print_fun(sched, ffsolo_allreduce_print);

    //create the activation schedule
    ffop_h activation_schedule_test, activation_schedule_link, activation_schedule_op, activation_schedule_root, activation_join;
    ffactivation(FFSHADOW_TAG, 0, &activation_schedule_op, &activation_schedule_test, &activation_join, &(state->activation_schedule));
    ffschedule_get_begin_op(state->activation_schedule, &activation_schedule_root);
    ffschedule_get_end_op(state->activation_schedule, &activation_schedule_link);
    
    //create the allreduce schedule
    ffop_h allreduce_begin, allreduce_end;  
    ffallreduce(sndbuff, rcvbuff, count, tag, operator, datatype, options, &(state->allreduce_schedule));
    ffschedule_get_begin_op(state->allreduce_schedule, &allreduce_begin);
    ffschedule_get_end_op(state->allreduce_schedule, &allreduce_end);
    
    //create the solo limiter
    ffop_h limiter_async, limiter_sync;
    ffsolo_limiter(async, &limiter_async, &limiter_sync, &(state->solo_limiter));

    //create test op: the activation of a solo allreduce completes if the activation schedule completes
    //OR the limiter bypass the activation schedule and goes to the (sync) allreduce directly 
    ffnop(FFOP_DEP_OR, &(state->allreduce_activation_test));
    ffop_hb(activation_schedule_test, state->allreduce_activation_test, FFDEP_IGNORE_VERSION);
    ffop_hb(limiter_sync, state->allreduce_activation_test, FFDEP_IGNORE_VERSION);

    //the limiter activates the activation schedule on its async path
    ffop_hb(limiter_async, activation_schedule_op, FFDEP_IGNORE_VERSION);

    //creae the activation op: the solo allreduce is activated by the activation schedule 
    //OR by the limiter directly if the sync version is wanted
    ffop_h allreduce_activate;
    ffnop(FFOP_DEP_OR, &allreduce_activate);
    ffop_hb(activation_schedule_link, allreduce_activate, FFDEP_IGNORE_VERSION);
    ffop_hb(limiter_sync, activation_join, FFDEP_IGNORE_VERSION);

    //allreduce activation op activates the allreduce
    ffop_hb(allreduce_activate, allreduce_begin, 0);

    //add the feedback link to re-execute the schedule once the current one completes
    ffop_hb(allreduce_end, activation_schedule_root, FFDEP_IGNORE_VERSION);

    //add the created ops to the schedule so we can delete it then
    ffschedule_add_op(sched, allreduce_activate);
    ffschedule_add_op(sched, state->allreduce_activation_test);

    //FIXME: TODO
    // 1) add multiple buffers

    *_sched = sched;
    return FFSUCCESS;  
}

int ffsolo_allreduce_start(ffschedule_h sched){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    return ffschedule_post(state->activation_schedule);
}

int ffsolo_allreduce_post(ffschedule_h sched){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    return ffschedule_post(state->solo_limiter);
}

int ffsolo_allreduce_wait(ffschedule_h sched){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    FFCALLV(ffop_wait(state->allreduce_activation_test), FFERROR);
    return ffschedule_wait(state->allreduce_schedule);
}

int ffsolo_allreduce_test(ffschedule_h sched, int * flag){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    ffop_test(state->allreduce_activation_test, flag);
    if (*flag){
        ffschedule_test(state->allreduce_schedule, flag);
    }
    return FFSUCCESS;
}   

int ffsolo_allreduce_print(ffschedule_h handle, FILE * fp, char * name){
    solo_allreduce_state_t * state;
    ffschedule_get_state(handle, (void **) &state);

    ffschedule_print(state->solo_limiter, fp, "limiter");
    ffschedule_print(state->activation_schedule, fp, "activation");
    ffschedule_print(state->allreduce_schedule, fp, "allreduce");
    ffschedule_default_print(handle, fp, name);

    return FFSUCCESS;
}

int ffsolo_allreduce_delete(ffschedule_h sched){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    ffschedule_delete(state->activation_schedule);
    ffschedule_delete(state->allreduce_schedule);
    ffschedule_delete(state->solo_limiter);
    ffschedule_default_delete(sched); //to free the ops inside the schedule and the schedule itself
    free(state);
    //no need to delete the ops in state since they are part of the activation schedule
    return FFSUCCESS;
}
