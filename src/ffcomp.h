#ifndef _FFCOMP_H_
#define _FFCOMP_H_

#include "ff.h"
#include "ffinternal.h"
#include "ffop_mem.h"
#include "common/gcomp/ffop_gcomp_operator.h"

typedef struct ffcomp{
    ffop_mem_t buffer1;
    ffop_mem_t buffer2;
    ffop_mem_t buffer3;

    ffoperator_h operator_type;    
#ifdef FFOP_MPI
    ffop_gcomp_operator_t operator;
#endif

} ffcomp_t;

#endif /* _FFCOMP_H_ */
