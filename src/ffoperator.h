#ifndef _FFOPERATOR_H_
#define _FFOPERATOR_H_

#define FFSUM    0
#define FFPROD   1
#define FFMAX    2
#define FFMIN    3
#define FFCUSTOM 4

typedef uint32_t ffoperator_t;

typedef void (*ffcustom_op_t)(void*, void*, uint32_t, ffdatatype_t);

typedef struct ffoperator_internal{
    ffoperator_t type;
    int commute;
    ffcustom_op_t op_fun;
} ffoperator_internal_t;


void ffcustom_op_create(ffcustom_op_t, int commute, ffoperator_t * op);
void ffcustom_op_delete(soperator_t op);


#endif /* _FFOPERATOR_H_ */
