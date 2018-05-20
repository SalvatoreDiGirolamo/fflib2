#ifndef _FFCOMP_H_
#define _FFCOMP_H_

#include "ffoperator.h"

typedef struct ffcomp{
    ffop_mem_t buffer1;
    ffop_mem_t buffer2;
    ffoperator_t op;
    uint8_t flags;

} ffcomp_t;

#endif /* _FFCOMP_H_ */
