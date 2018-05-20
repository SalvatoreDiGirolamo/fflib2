#ifndef _FF_H_
#define _FF_H_

#define FFSUCCESS 0
#define FFERROR -1
#define FFINVALID_ARG -2
#define FFTOO_MANY_DEPS -3
#define FFENOMEM -4

#include <stddef.h>

#define FFOP_MPI

#define FFINT32 0
#define FFINT64 1
#define FFDOUBLE 2
#define FFFLOAT 3
#define FFCHAR 4
#define FFDATATYPE_SENTINEL 5

typedef int ffdatatype_t;
typedef void * ffop_h;

int ffinit(int * argc, char *** argv);
int fffinalize();

int ffrank(int * rank);
int ffsize(int * size);

int ffop_post(ffop_h op);
int ffop_wait(ffop_h op);
int ffop_test(ffop_h op, int * flag);
int ffop_happens_before(ffop_h first, ffop_h second);

int ffsend(void * buffer, int count, ffdatatype_t datatype, int dest, int tag, int options, ffop_h * op);
int ffrecv(void * buffer, int count, ffdatatype_t datatype, int source, int tag, int options, ffop_h * op);


#endif /* _FF_H_ */
