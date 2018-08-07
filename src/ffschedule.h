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
    ffbuffer_h * tmp_buffers;
    uint32_t tmp_buffers_num;
} ffschedule_t;


int ffschedule_init();


#endif /* _FFSCHEDULE_H_ */
