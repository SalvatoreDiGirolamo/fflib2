#ifndef _FFOP_H_
#define _FFOP_H_

#define MAX_DEPS 10

#define FFPOLLS_BEFORE_YIELD 1000

#define FFOP_ENQUEUE(op, oplist) { \
    op->next = *oplist; \
    *oplist = op; \
}

#define FFOP_COMPLETED(op){ \
    __sync_add_and_fetch(&(op->completed), 1); \
} 


//#include "ffinternal.h"
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

int ffop_init();
int ffop_finalize();
int ffop_create(ffop_t ** ptr);
int ffop_complete(ffop_t * op);

#endif /* _FFOP_H_ */
