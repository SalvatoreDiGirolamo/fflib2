
#include "ff.h"
#include "ffop.h"
#include "ffprogress.h"
#include "ffinternal.h"


static ffdescr_t ff;

int ff_init(int * argc, char *** argv){

    int ret; 

    ffop_init();
    ff.terminate = 0;
    ret = pthread_create(&(ff.progress_thread), NULL, progress_thread, &ff);
    if (ret){ return FFERROR; }

    return FFSUCCESS;
}


int ff_finalize(){
  

    ff.terminate=1;
    if (pthread_join(ff.progress_thread, NULL)){
        return FFERROR;
    }
    
    return FFSUCCESS;

}
