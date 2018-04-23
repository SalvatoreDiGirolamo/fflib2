#ifndef _FFOP_H_
#define _FFOP_H_

#define MAX_DEPS 10

#define FFSEND 0
#define FFRECV 1
#define FFCOMP 2
#define FFMAX_IDX 3 /* not actual op, just boundary for ops indexes */

typedef int (*ffop_post_t)(ffop_t*, ffop_mem_set_t*);
typedef int (*ffop_wait_t)(ffop_t*);
typedef int (*ffop_free_t)(ffop_t*);
typedef int (*ffop_init_t)(ffop_t*);



typedef ffop_descriptor{
    ffop_init_t init;
    ffop_post_t post;
    ffop_free_t free;
    ffop_test_t test;
} ffop_descriptor_t;

typedef uint32_t ffop_type_t;

typedef struct ffop{
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


} ffop_t;


int ffop_init();


#endif /* _FFOP_H_ */
