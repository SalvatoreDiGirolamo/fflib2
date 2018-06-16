#ifndef _FFNOP_H_
#define _FFNOP_H_

#include "ffop_mem.h"
#include "ffinternal.h"

typedef struct ffop ffop_t;

typedef struct ffnop{
    /* nothing for now */
} ffnop_t;

int ffnop_post(ffop_t * op, ffop_mem_set_t * mem);

#endif /* _FFNOP_H_ */
