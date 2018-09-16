#include "ff.h"
#include "ffprogress.h"
#include "ffinternal.h"
#include "ffop_scheduler.h"
#include "ffop.h"

volatile int _progresser_ready = 0;

static ffprogresser_t progressers[MAX_PROGRESSERS];
static uint32_t progressers_count = 0;

int ffprogresser_register(ffprogresser_t progresser){
    if (progressers_count >= MAX_PROGRESSERS) {
        FFLOG_ERROR("Not enough progressers slots!\n");
        return FFENOMEM;
    }
    
    progressers[progressers_count++] = progresser;
    return FFSUCCESS;
}

int progresser_ready(){
    return _progresser_ready;
}

void * progress_thread(void * args){

    ffdescr_t * ff = (ffdescr_t *) args;

    /* Initialize the progresses */
    for (uint32_t i=0; i<progressers_count; i++){
        FFCALLV(progressers[i].init(), NULL);
    }

    _progresser_ready = 1;
    while (!ff->terminate){
        ffop_t * completed = NULL;        

        /* Execute new operations */
        FFCALLV(ffop_scheduler_execute_all(), NULL);

        /* Call the progressers */
        for (uint32_t i=0; i<progressers_count; i++){
            FFCALLV(progressers[i].progress(&completed), NULL);
        }

        /* Satisfy the dependencies */
        while (completed!=NULL){ 
            ffop_complete(completed);
            completed = completed->instance.next;
        }
    }       

    /* Finalize the progressers */
    for (uint32_t i=0; i<progressers_count; i++){
        FFCALLV(progressers[i].finalize(), NULL);
    }

    return NULL;
}
