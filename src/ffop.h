#ifndef _FFOP_H_
#define _FFOP_H_

#define MAX_DEPS 10

#define FFSEND 0
#define FFRECV 1
#define FFCOMP 2
#define FFMAX_IDX 3 /* not actual op, just boundary for ops indexes */

#define FFPOLLS_BEFORE_YIELD 1000

#include "ffinternal.h"
#include "ffsend.h"
#include "ffrecv.h"
#include "ffcomp.h"

typedef uint32_t ffop_type_t;

struct ffop{
    ffop_type_t type;

    /* actual operation */
    union{
        ffsend_t send;
        ffrecv_t recv;
        ffcomp_t comp;
    };

    /* pointers to operations dependent on this */
    struct ffop * dependent[MAX_DEPS];

    /* number of dependent operations */
    uint32_t out_dep_count;

    /* number of incoming dependencies (fired when 0) */
    uint32_t in_dep_count;

    /* used in schedule_t for determining the next executable op */
    struct ffop * next;

    /* flag that is set to true (!=0) if the op is completed */
    volatile uint8_t completed;

    /* flag that is set to true (!=0) if the op has been posted */
    volatile uint8_t posted;

};

typedef int (*ffop_post_t)(ffop_t*, ffop_mem_set_t*);
typedef int (*ffop_init_t)(ffop_t*);

typedef struct ffop_descriptor{
    ffop_init_t init;
    ffop_post_t post;
} ffop_descriptor_t;


extern ffop_descriptor_t ops[FFMAX_IDX];

int ffop_init();
int ffop_finalize();
int ffop_create(ffop_t ** ptr);

#endif /* _FFOP_H_ */
