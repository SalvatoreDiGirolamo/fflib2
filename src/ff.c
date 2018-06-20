
#include "ff.h"
#include "ffop.h"
#include "ffnop.h"
#include "ffstorage.h"
#include "ffprogress.h"
#include "ffinternal.h"
#include "ffschedule.h"

#ifdef FFDEBUG
#include <sys/types.h>
#include <unistd.h>
#endif

ffdescr_t ff;

int ffinit(int * argc, char *** argv){

    int ret; 

    ffbind(&ff);
        
    /* ffnop is internal */
    ff.impl.ops[FFNOP].init = NULL;
    ff.impl.ops[FFNOP].post = ffnop_post; 
    
    ffstorage_init();
    ffop_init();
    ffschedule_init();

    ff.impl.init(argc, argv);

    ff.terminate = 0;
    ret = pthread_create(&(ff.progress_thread), NULL, progress_thread, &ff);
    if (ret){ return FFERROR; }
    while (!progresser_ready());

#ifdef FFDEBUG
    ffrank(&dbg_myrank);
#endif

    FFLOG("Init: %i ==> PID %u\n", dbg_myrank, getpid());

    return FFSUCCESS;
}


int fffinalize(){

    ff.terminate=1;
    if (pthread_join(ff.progress_thread, NULL)){
        return FFERROR;
    }
 
    ff.impl.finalize(); 
    ffop_finalize();
    ffstorage_finalize();
  
    return FFSUCCESS;
}

int ffrank(int * rank){
    return ff.impl.get_rank(rank);
}

int ffsize(int * size){
    return ff.impl.get_size(size);
}

