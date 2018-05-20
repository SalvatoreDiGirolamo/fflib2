#ifndef _FF_H_
#define _FF_H_

#define FFSUCCESS 0
#define FFERROR -1
#define FFINVALID_ARG -2
#define FFTOO_MANY_DEPS -3
#define FFENOMEM -4

#include <stddef.h>

#define FFOP_MPI

typedef void * ffop_h;

int ff_init(int * argc, char *** argv);
int ff_finalize();

int ffop_post(ffop_h op);
int ffop_wait(ffop_h op);
int ffop_test(ffop_h op, int * flag);
int ffop_happens_before(ffop_h first, ffop_h second);


int ffsend(void * buffer, size_t size, int dest, int tag, int options, ffop_h * op);
int ffrecv(void * buffer, size_t size, int source, int tag, int options, ffop_h * op);





#endif /* _FF_H_ */
