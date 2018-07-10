#ifndef _FF_H_
#define _FF_H_

#define FFCOMPLETED 1
#define FFSUCCESS 0
#define FFERROR -1
#define FFINVALID_ARG -2
#define FFTOO_MANY_DEPS -3
#define FFENOMEM -4

#include <stddef.h>
#include <stdint.h>

#define FFOP_MPI

/* datatypes */
#define FFINT32             0
#define FFINT32_CTYPE       int32
#define FFINT64             1
#define FFINT64_CTYPE       int64
#define FFDOUBLE            2
#define FFDOUBLE_CTYPE      double
#define FFFLOAT             3
#define FFFLOAT_CTYPE       float
#define FFCHAR              4
#define FFCHAR_CTYPE        char
#define FFDATATYPE_SENTINEL 5


/* operators */
#define FFSUM               0
#define FFPROD              1
#define FFMAX               2
#define FFMIN               3
#define FFIDENTITY          4
#define FFOPERATOR_SENTINEL 5
#define FFCUSTOM            6

/* options */
#define FFOP_DEP_AND        0x001
#define FFOP_DEP_OR         0x002
#define FFOP_NON_PERSISTENT 0x003
#define FFCOMP_DEST_ATOMIC  0x004


/* Our NULL */
#define FFNONE              -1

typedef int ffdatatype_h;
typedef int ffoperator_h;
typedef uint64_t ffop_h;
typedef uint64_t ffschedule_h;

int ffinit(int * argc, char *** argv);
int fffinalize();

int ffrank(int * rank);
int ffsize(int * size);

int ffop_post(ffop_h op);
int ffop_wait(ffop_h op);
int ffop_test(ffop_h op, int * flag);
int ffop_hb(ffop_h first, ffop_h second);
int ffop_free(ffop_h _op);
int ffop_tostring(ffop_h op, char * str, int len);

int ffsend(void * buffer, int count, ffdatatype_h datatype, int dest, int tag, int options, ffop_h * op);
int ffrecv(void * buffer, int count, ffdatatype_h datatype, int source, int tag, int options, ffop_h * op);


typedef int (*ffoperator_fun_t)(void*, void*, void*, uint32_t, ffdatatype_h);
int ffcomp(void * buff1, void * buff2, int count, ffdatatype_h datatype, ffoperator_h ffoperator, int options, void * buff3, ffop_h * op);
int ffcomp_operator_create(ffoperator_fun_t fun, int commutative, ffoperator_h * handle);
int ffcomp_operator_delete(ffoperator_h handle);

int ffnop(int options, ffop_h * handle);

int ffschedule_create(ffschedule_h *sched);
int ffschedule_delete(ffschedule_h sched);
int ffschedule_add_op(ffschedule_h sched, ffop_h op); 
int ffschedule_post(ffschedule_h sched);
int ffschedule_wait(ffschedule_h handle);
int ffschedule_test(ffschedule_h handle, int * flag);
int ffschedule_set_tmpmem(ffschedule_h handle, void * mem);


int ffallreduce(void * sndbuff, void * rcvbuff, int count, int tag, ffoperator_h ffoperator, ffdatatype_h datatype, ffschedule_h * _sched);


#endif /* _FF_H_ */
