#include "ff.h"
#include "ffprogress.h"
#include "ffinternal.h"
#include "ffop.h"
#include "mpi/ffop_mpi_progresser.h"

void * progress_thread(void * args){

    ffdescr_t * ff = (ffdescr_t *) args;

    /* Initialize the progresses */
    ffop_mpi_progresser_init();

    while (!ff->terminate){
        ffop_t * completed = NULL;        

        /* Call the progressers */
        
        //MPI
        FFCALLV(ffop_mpi_progresser_progress(&completed), NULL);

        //Others...
    

        /* Satisfy the dependencies */
        while (completed!=NULL){ 
            ffop_complete(completed);
            completed = completed->next;
        }
    }       

    /* Finalize the progressers */
    FFCALLV(ffop_mpi_progresser_finalize(), NULL);

    return NULL;
}
