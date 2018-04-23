#ifndef _FF_H_
#define _FF_H_

#define FFSUCCESS MPI_SUCCESS
#define FFINVALID_ARG -1
#define FFTOO_MANY_DEPS -2

typedef void * ffop_h;

int ffop_post(ffop_h op);
int ffop_wait(ffop_h op);
int ffop_test(ffop_h op);
int ffop_happens_before(ffop_h first, ffop_h second);


int ffsend(void * buffer, size_t size, int source, int tag, MPI_Comm comm int options, ffop_t * op);
int ffrecv(void * buffer, size_t size, int source, int tag, MPI_Comm comm int options, ffop_t * op);




#endif /* _FF_H_ */
