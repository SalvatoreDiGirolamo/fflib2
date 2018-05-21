#ifndef _FFINTERNAL_
#define _FFINTERNAL_

#include "ff.h"

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>

#define FFCALL(X) { int r; if (r=(X)!=FFSUCCESS) return r; }
#define FFCALLV(X, V) { int r; if (r=(X)!=FFSUCCESS) return V; }

#define FFLOG_ERROR(MSG) { printf("FFlib error: %s\n", MSG); }

typedef uint32_t ffpeer_t;
typedef struct ffop ffop_t;

typedef int (*ffimpl_init_t)(int*, char ***);
typedef int (*ffimpl_finalize_t)();
typedef int (*ffimpl_get_rank_t)(int*);
typedef int (*ffimpl_get_size_t)(int*);

typedef struct ff_descr{
    volatile int terminate;
    volatile int progress_thread_ready;
    pthread_t progress_thread;

    ffimpl_init_t impl_init;
    ffimpl_finalize_t impl_finalize;
    ffimpl_get_rank_t impl_get_rank;
    ffimpl_get_size_t impl_get_size;
    
} ffdescr_t;



#endif /* _FFINTERNAL_ */
