#ifndef _FF_H_
#define _FF_H_

#define FFSUCCESS 0
#define FFERROR -1
#define FFINVALID_ARG -2
#define FFTOO_MANY_DEPS -3
#define FFENOMEM -4

#include <stddef.h>
#include <stdint.h>

#define FFOP_MPI

/* datatypes */
#define FFINT32 0
#define FFINT64 1
#define FFDOUBLE 2
#define FFFLOAT 3
#define FFCHAR 4
#define FFDATATYPE_SENTINEL 5


/* operators */
#define FFSUM    0
#define FFPROD   1
#define FFMAX    2
#define FFMIN    3
#define FFOPERATOR_SENTINEL 4

/* Our NULL */
#define FFNONE -1

typedef int ffdatatype_h;
typedef int ffoperator_h;
typedef uint64_t ffop_h;

int ffinit(int * argc, char *** argv);
int fffinalize();

int ffrank(int * rank);
int ffsize(int * size);

int ffop_post(ffop_h op);
int ffop_wait(ffop_h op);
int ffop_test(ffop_h op, int * flag);
int ffop_hb(ffop_h first, ffop_h second);
int ffop_free(ffop_h _op);

int ffsend(void * buffer, int count, ffdatatype_h datatype, int dest, int tag, int options, ffop_h * op);
int ffrecv(void * buffer, int count, ffdatatype_h datatype, int source, int tag, int options, ffop_h * op);


typedef int (*ffoperator_fun_t)(void*, void*, void*, uint32_t, ffdatatype_h);
int ffcomp(void * buff1, void * buff2, int count, ffdatatype_h datatype, ffoperator_h ffoperator, int options, void * buff3, ffop_h * op);
int ffcomp_operator_create(ffoperator_fun_t fun, int commutative, ffoperator_h * handle);
int ffcomp_operator_delete(ffoperator_h handle);




#endif /* _FF_H_ */
