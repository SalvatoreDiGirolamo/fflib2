#ifndef _FFINTERNAL_
#define _FFINTERNAL_

#include "ff.h"
#include "ffdatatype.h"
#include "fflocks.h"

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>

#include <unistd.h>
#define FFCALL(X) { int r; if (r=(X)!=FFSUCCESS) return r; }
#define FFCALLV(X, V) { int r; if (r=(X)!=FFSUCCESS) return V; }

//#define FFLOG_ERROR(MSG, ...) { printf("FFlib error: %s"MSG, ## __VA_ARGS__); }

#define FFLOG_ERROR(MSG, ...) { printf("[%u][%s:%i] "MSG, getpid(), __FILE__, __LINE__,  ##__VA_ARGS__); }

#define IS_OPT_SET(op, opt) (op->options & opt == opt)

#ifdef FFDEBUG
extern int dbg_myrank;
#define FFLOG(MSG, ...) printf("[%i - %u][%s:%i] "MSG, dbg_myrank, getpid(), __FILE__, __LINE__,  ##__VA_ARGS__)

#define DOTLEN 256
#define FFGRAPH(A, B) \
{ \
char dot_a[DOTLEN]; \
char dot_b[DOTLEN]; \
ffop_tostring(A, dot_a, DOTLEN); \
ffop_tostring(B, dot_b, DOTLEN); \
FFLOG("DOT#\"%i.%s\" -> \"%i.%s\"\n", dbg_myrank, dot_a, dbg_myrank, dot_b); \
}
#else
#define FFLOG(MSG, ...) 
#define FFGRAPH(A, B) 
#endif


#define MIN(a, b, c) (((a<b ? a : b) < c) ? (a<b ? a : b) : c)

#define FFSEND    0
#define FFRECV    1
#define FFCOMP    2
#define FFNOP     3
#define FFMAX_IDX 4 /* not actual op, just boundary for ops indexes */

typedef int ffdatatype_t; /* for now the internal datatype type is an int as well */
typedef uint32_t ffpeer_t;
typedef struct ffop ffop_t;
typedef struct ffop_descriptor ffop_descriptor_t;
typedef struct ffop_mem_set ffop_mem_set_t;

typedef int (*ffimpl_init_t)(int*, char***);
typedef int (*ffimpl_finalize_t)();
typedef int (*ffimpl_get_rank_t)(int*);
typedef int (*ffimpl_get_size_t)(int*);
typedef int (*ffimpl_operator_create_t)(ffoperator_fun_t, int, ffoperator_h*);
typedef int (*ffimpl_operator_delete_t)(ffoperator_h);


/* Operation descriptor */
typedef int (*ffop_post_t)(ffop_t*, ffop_mem_set_t*);
typedef int (*ffop_init_t)(ffop_t*);
typedef int (*ffop_tostring_t)(ffop_t*, char * str, int len);
typedef int (*ffop_finalize_t)(ffop_t*);

typedef struct ffop_descriptor{
    ffop_init_t init;
    ffop_post_t post;
    ffop_tostring_t tostring;
    ffop_finalize_t finalize;
} ffop_descriptor_t;


/* FF descriptor */
typedef struct ff_descr{
    volatile int terminate;
    volatile int progress_thread_ready;
    pthread_t progress_thread;

    struct impl{
        ffimpl_init_t init;
        ffimpl_finalize_t finalize;
        ffimpl_get_rank_t get_rank;
        ffimpl_get_size_t get_size;
        ffimpl_operator_create_t operator_create;
        ffimpl_operator_delete_t operator_delete;
        ffop_descriptor_t ops[FFMAX_IDX];
    } impl;
} ffdescr_t;


extern ffdescr_t ff;

#endif /* _FFINTERNAL_ */
