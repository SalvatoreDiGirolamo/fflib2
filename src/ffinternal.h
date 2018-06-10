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

#ifdef FFDEBUG
static int dbg_myrank;
#define FFLOG(MSG, ...) printf("[%i][%s:%i] "MSG, dbg_myrank, __FILE__, __LINE__,  ##__VA_ARGS__)
#else
#define FFLOG(MSG, ...) 
#endif

#define MIN(a, b, c) (((a<b ? a : b) < c) ? (a<b ? a : b) : c)


typedef int ffdatatype_t; /* for now the internal datatype type is an int as well */
typedef uint32_t ffpeer_t;
typedef struct ffop ffop_t;
typedef struct ffop_descriptor ffop_descriptor_t;

typedef int (*ffimpl_init_t)(int*, char ***);
typedef int (*ffimpl_finalize_t)();
typedef int (*ffimpl_get_rank_t)(int*);
typedef int (*ffimpl_get_size_t)(int*);
typedef int (*ffimpl_register_op_t)(int, ffop_descriptor_t*);


typedef struct ff_descr{
    volatile int terminate;
    volatile int progress_thread_ready;
    pthread_t progress_thread;

    struct impl{
        ffimpl_init_t init;
        ffimpl_finalize_t finalize;
        ffimpl_get_rank_t get_rank;
        ffimpl_get_size_t get_size;
        ffimpl_register_op_t register_op; 
    } impl;
} ffdescr_t;


extern ffdescr_t ff;

#endif /* _FFINTERNAL_ */
