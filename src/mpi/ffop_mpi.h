#ifndef _FFOP_MPI_
#define _FFOP_MPI_

#include <mpi.h>

#include "../ff.h"
#include "../ffop_mem.h"

typedef struct ffop ffop_t;

/* Max outstanding MPI operations */
#define FFMPI_MAX_REQ 256

typedef struct mpi_transport {
    MPI_Request mpireq;
} mpi_transport_t;

int ffop_mpi_init(ffop_t * op);
int ffop_mpi_send_post(ffop_t * op, ffop_mem_set_t * mem);
int ffop_mpi_recv_post(ffop_t * op, ffop_mem_set_t * mem);

extern MPI_Datatype datatype_translation_table[FFDATATYPE_SENTINEL];

#endif /* _FFOP_MPI_ */
