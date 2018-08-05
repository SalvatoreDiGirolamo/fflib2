#ifndef _FFOP_GCOMP_H_
#define _FFOP_GCOMP_H_

int ffop_gcomp_init(ffop_t * op);
int ffop_gcomp_post(ffop_t * op, ffbuffer_set_t * mem);

int ffop_gcomp_wait(ffop_t * op);
int ffop_gcomp_test(ffop_t * op, int * flag);

#endif /* _FFOP_GCOMP_H_ */
