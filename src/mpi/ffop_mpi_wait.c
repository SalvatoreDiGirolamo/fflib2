
#include "ffop_mpi.h"
#include "ffop_mpi_progresser.h"

int ffop_mpi_wait(ffop_t * op){
#ifdef CHECK_ARGS
    if (op==NULL) return FFINVALID_ARG;
#endif

    MPI_Request * req;
    if (op == FFSEND) req = &(op->send.transport.mpireq);
    else req = &(op->recv.transport.mpireq);

    int res = MPI_Wait(req, MPI_STATUS_IGNORE);
    if (res!=MPI_SUCCESS) return FFERROR;

    return ffop_mpi_progresser_release(op);
}

