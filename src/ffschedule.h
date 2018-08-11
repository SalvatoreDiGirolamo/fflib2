#ifndef _FFSCHEDULE_H_
#define _FFSCHEDULE_H_

#include "ff.h"
#include "ffinternal.h"
#include "ffop.h"

typedef struct ffschedule{
    ffop_t * oplist;
    ffop_t * begin_op;
    ffop_t * end_op;
    uint64_t id;
    void * state;

    ffschedule_post_callback_t post_callback;
    ffschedule_delete_callback_t delete_callback;
} ffschedule_t;


int ffschedule_init();


#endif /* _FFSCHEDULE_H_ */
