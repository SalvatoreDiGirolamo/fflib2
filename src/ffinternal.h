#ifndef _FFINTERNAL_
#define _FFINTERNAL_

#include "ff.h"

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define FFCALL(X) { int r; if (r=(X)!=FFSUCCESS) return r; }
#define FFCALLV(X, V) { int r; if (r=(X)!=FFSUCCESS) return V; }



typedef uint32_t ffpeer_t;

typedef struct ffop ffop_t;

typedef struct ff_descr{
    volatile int terminate;
    pthread_t progress_thread;

} ffdescr_t;



#endif /* _FFINTERNAL_ */
