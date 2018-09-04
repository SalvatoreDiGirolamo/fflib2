#include "ffcollectives.h"
#include <assert.h>

int ffactivation_free(ffschedule_h sched){
    int * buff;
    ffschedule_get_state(sched, (void **) &buff);
    free(buff);
}

int ffactivation(int tag, ffop_h *user_activator, ffop_h * user_activator_test, ffschedule_h *_sched){
    ffschedule_h sched;
    FFCALL(ffschedule_create(&sched));

    int csize, rank;
    ffsize(&csize);
    ffrank(&rank);

    ffop_h completion, sched_activation;

    int * buff = malloc(sizeof(int));

    int maxr = (int)ceil((log2(csize)));
    ffop_h * recvs = (ffop_h *) malloc(sizeof(ffop_h)*maxr);
    ffop_h * completions = (ffop_h *) malloc(sizeof(ffop_h)*maxr);
    
    ffschedule_set_state(sched, (void *) buff);
    ffschedule_set_delete_callback(sched, ffactivation_free);

    ffop_h sched_begin_op;
    ffschedule_get_begin_op(sched, &sched_begin_op);   

    ffnop(0, user_activator); 
    ffop_hb(sched_begin_op, *user_activator, FFDEP_NO_AUTOPOST);

    ffop_h prev_dep = *user_activator;
    int mask = 0x1, cnt=0;
    while (mask < csize) {
        uint32_t dst = rank^mask;
        if (dst < csize) {
            ffop_h send, recv, completion;

            //comp 
            ffnop(0, &completion);
            if (prev_dep!=FFNONE) ffop_hb(prev_dep, completion, 0);

            //send
            ffsend(buff, 1, FFINT32, dst, tag, FFOP_DEP_OR, &send);
            for (int i=0; i<cnt; i++){
                ffop_hb(recvs[i], send, 0);
                ffop_hb(send, completions[i], 0);
            }            
            ffop_hb(*user_activator, send, 0);
            ffop_hb(send, completion, 0);

            //recv
            ffrecv(buff, 1, FFINT32, dst, tag, 0, &recv);

            recvs[cnt] = recv;
            completions[cnt] = completion;
            cnt++;
            prev_dep = recv;

            ffschedule_add_op(sched, send);
        }
        mask <<= 1;
    }

    if (cnt>0) *user_activator_test = completions[0];

    ffschedule_add_op(sched, *user_activator);
    ffop_h sched_completion;
    ffnop(FFOP_DEP_OR, &sched_completion);

    ffop_hb(recvs[cnt-1], sched_completion, 0);

    for (int i=0; i<cnt; i++){     
        ffop_hb(completions[i], sched_completion, 0);
        //this thing that we need to add the ops to the sched once all the deps are satisfied sucks!!!
        //FIXME: ffop_hb should be able to update the schedule (if it exists)
        ffschedule_add_op(sched, recvs[i]);
        ffschedule_add_op(sched, completions[i]);
    }
    ffschedule_add_op(sched, sched_completion);


    *_sched = sched;

    free(recvs);
    free(completions);

    return FFSUCCESS;
}
