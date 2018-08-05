#ifndef _FFOP_MPI_
#define _FFOP_MPI_

#include <mpi.h>

#include "ff.h"
#include "ffbuffer.h"

typedef struct ffop ffop_t;

/* Max outstanding MPI operations */
#define FFMPI_MAX_REQ 256

int ffop_mpi_init(ffop_t * op);

int ffop_mpi_send_post(ffop_t * op, ffbuffer_set_t * mem);
int ffop_mpi_send_wait(ffop_t * op);
int ffop_mpi_send_test(ffop_t * op, int * flag);

int ffop_mpi_recv_post(ffop_t * op, ffbuffer_set_t * mem);
int ffop_mpi_recv_wait(ffop_t * op);
int ffop_mpi_recv_test(ffop_t * op, int * flag);

extern MPI_Datatype datatype_translation_table[FFDATATYPE_SENTINEL];

#endif /* _FFOP_MPI_ */
