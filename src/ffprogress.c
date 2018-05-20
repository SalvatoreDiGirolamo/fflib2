#include "ffprogress.h"
#include "ffinternal.h"

#include "mpi/ffop_mpi_progresser.h"

void * progress_thread(void * args){

    ffdescr_t * ff = (ffdescr_t *) args;

    /* Initialize the progressers */
    FFCALLV(ffop_mpi_progresser_init(), NULL);

    while (!ff->terminate){
        ffop_t * completed = NULL;        

        /* Call the progressers */
        FFCALLV(ffop_mpi_progresser_progress(&completed), NULL);


        /* Satisfy the dependencies */
        while (completed!=NULL){            
            for (int i=0; i<completed->out_dep_count; i++){
                ffop_t * dep_op = completed->dependent[i];

                uint32_t deps = __sync_add_and_fetch(&(dep_op->in_dep_count), -1);
                if (deps==0){
                    ffop_post(dep_op);
                }
            }

            completed = completed->next;
        }
    }       


    /* Initialize the progressers */
    FFCALLV(ffop_mpi_progresser_finalize(), NULL);

    return NULL;
}
