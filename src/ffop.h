#ifndef _FFOP_H_
#define _FFOP_H_

#define MAX_DEPS 10

#define FFPOLLS_BEFORE_YIELD 5

#define FFOP_ENQUEUE(op, oplist) { \
    op->instance.next = *oplist; \
    *oplist = op; \
}


//#define FFOP_IS_COMPLETED(op) (op->instance.posted_version > op->version && op->instance.completed_version == op->instance.posted_version)


#define FFOP_IS_COMPLETED(op) (op->instance.completed)

//#include "ffinternal.h"
#include "ffsend.h"
#include "ffrecv.h"
#include "ffcomp.h"
#include "ffnop.h"

typedef uint32_t ffop_type_t;

struct ffop{
    ffop_type_t type;
    
    uint64_t id;

    /* actual operation */
    union{
        ffsend_t send;
        ffrecv_t recv;
        ffcomp_t comp;
        ffnop_t  nop;
    };

    /* pointers to operations dependent on this */
    struct ffop * dependent[MAX_DEPS];

    /* number of dependent operations */
    uint32_t out_dep_count;

    /* number of incoming dependencies (fired when 0) */
    uint32_t in_dep_count;

    /* options */
    uint32_t options;

    /* next operation in the current schedule. It is meaningful only if 
     * the operations belongs to a schedule. */
    struct ffop * sched_next;

    /* current version of the op that has to be posted */
    volatile uint32_t version;

    /* this is the consumable part that gets reset every time the operation
     * has to be re-executed. */
    struct instance{
        /* current number of in-deps that need to be satisfied before 
         * triggering this operaiton */
        uint32_t dep_left;

        /* used for the readylist by the progressers */
        struct ffop * next; 

        /* version of the op that has been completed */
        //volatile uint32_t completed_version;

        /* version of the op that has been posted*/
        //volatile uint32_t posted_version;

        /* completion flag */
        volatile uint8_t completed;
    } instance;

#ifdef WAIT_COND
    FFMUTEX_TYPE mutex;
    FFCOND_TYPE cond;
#endif

};

int ffop_init();
int ffop_finalize();
int ffop_create(ffop_t ** ptr);
int ffop_complete(ffop_t * op);

#endif /* _FFOP_H_ */
