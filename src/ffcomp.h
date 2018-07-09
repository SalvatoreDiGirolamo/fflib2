#ifndef _FFCOMP_H_
#define _FFCOMP_H_

#include "ff.h"
#include "ffinternal.h"
#include "ffop_mem.h"
#include "ffbind.h"

typedef struct ffcomp{
    ffop_mem_t buffer1;
    ffop_mem_t buffer2;
    ffop_mem_t buffer3;

    ffoperator_h operator_type; 

    ffimpl_comp_data_t operator;

} ffcomp_t;

int ffcomp_tostring(ffop_t * op, char * str, int len);

int ffcomp_finalize(ffop_t * op);


#endif /* _FFCOMP_H_ */
