#ifndef _FFOP_MPI_
#define _FFOP_MPI_

#include <ffop_mpi_send.h>
#include <ffop_mpi_recv.h>

typedef struct mpi_transport {
    uint32_t idx;
    MPI_Request mpireq;
} mpi_transport_t;

int ffop_mpi_wait(ffop_t * op);
int ffop_mpi_test(ffop_t * op);
int ffop_mpi_send_post(ffop_t * op, ffop_mem_set_t * mem);
int ffop_mpi_recv_post(ffop_t * op, ffop_mem_set_t * mem);

#endif
