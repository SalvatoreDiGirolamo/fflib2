#ifndef _FFOPERATOR_H_
#define _FFOPERATOR_H_

#include "ffdatatype.h"

#define FFSUM    0
#define FFPROD   1
#define FFMAX    2
#define FFMIN    3
#define FFCUSTOM 4


typedef void (*ffoperator_fun_t)(void*, void*, uint32_t, ffdatatype_t);

typedef struct ffoperator_t{
    uint32_t type;
    int commute;
    ffoperator_fun_t op_fun;
} ffoperator_t;


void ffcustom_op_create(ffoperator_fun_t fun, int commute, ffoperator_t * op);
void ffcustom_op_delete(ffoperator_t op);


#endif /* _FFOPERATOR_H_ */
