#include "ffcollectives.h"

typedef struct solo_allreduce_state{
    ffschedule_h activation_schedule;
    ffschedule_h allreduce_schedule;
    ffop_h activation_op;    //op that has to be posted to activate the schedule locally
    ffop_h activation_test;  //op that needs to be tested to check if a local activation completed (two activations should not overlap)
} solo_allreduce_state_t;

int ffsolo_allreduce_post(ffschedule_h sched);
int ffsolo_allreduce_wait(ffschedule_h sched);
int ffsolo_allreduce_test(ffschedule_h sched, int * flag);
int ffsolo_allreduce_delete(ffschedule_h sched);

int ffsolo_allreduce(void * sndbuff, void * rcvbuff, int count, int16_t tag, ffoperator_h operator, ffdatatype_h datatype, int options, int async, ffschedule_h * _sched){

    solo_allreduce_state_t * state = (solo_allreduce_state_t *) malloc(sizeof(solo_allreduce_state_t));

    ffschedule_h sched;
    FFCALL(ffschedule_create(&sched));
    ffschedule_set_state(sched, state);
 
    ffschedule_set_post_fun(sched, ffsolo_allreduce_post);
    ffschedule_set_wait_fun(sched, ffsolo_allreduce_wait);
    ffschedule_set_test_fun(sched, ffsolo_allreduce_test);
    ffschedule_set_delete_fun(sched, ffsolo_allreduce_delete);
   
    ffop_h activation_link;  //op that is automatically posted when the activation schedule completes (locally or remotely)    
    ffop_h activation_root;  //activation schedule root op

    ffop_h allreduce_begin;  //allreduce schedule root op
    ffop_h allreduce_end;    //allreduce schedule end op

    //create the activation schedule
    ffactivation(0, &(state->activation_op), &(state->activation_test), &(state->activation_schedule));
    ffschedule_get_begin_op(state->activation_schedule, &activation_root);
    ffschedule_get_end_op(state->activation_schedule, &activation_link);
    
    //create the allreduce schedule
    ffallreduce(sndbuff, rcvbuff, count, tag, operator, datatype, options, &(state->allreduce_schedule));
    ffschedule_get_begin_op(state->allreduce_schedule, &allreduce_begin);
    ffschedule_get_end_op(state->allreduce_schedule, &allreduce_end);
    
    //the allreduce schedule is activated by the activation schedule
    ffop_hb(activation_link, allreduce_begin, 0);

    //add the feedback link to re-execute the schedule once the current one completes
    ffop_hb(allreduce_end, activation_root, FFDEP_IGNORE_VERSION);

    //FIXME: TODO
    // 1) add multiple buffers
    // 2) add solo async limiter (giving sense to the async parameter)

    *_sched = sched;
    return FFSUCCESS;  
}

int ffsolo_allreduce_post(ffschedule_h sched){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    return ffop_post(state->activation_op);
}

int ffsolo_allreduce_wait(ffschedule_h sched){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    FFCALLV(ffop_wait(state->activation_test), FFERROR);
    return ffschedule_wait(state->allreduce_schedule);
}

int ffsolo_allreduce_test(ffschedule_h sched, int * flag){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    ffop_test(state->activation_test, flag);
    if (*flag){
        ffschedule_test(state->allreduce_schedule, flag);
    }
    return FFSUCCESS;
}   

int ffsolo_allreduce_delete(ffschedule_h sched){
    solo_allreduce_state_t * state;
    ffschedule_get_state(sched, (void **) &state);
    ffschedule_delete(state->activation_schedule);
    ffschedule_delete(state->allreduce_schedule);
    //no need to delete the ops in state since they are part of the activation schedule
    return FFSUCCESS;
}

