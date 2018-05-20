
#include "ffop_mpi.h"
#include "ffop_mpi_progresser.h"


int ffop_mpi_test(ffop_t * op, int *_flag){
#ifdef CHECK_ARGS
    if (op==NULL) return FFINVALID_ARG;
#endif

    int flag, res;

    MPI_Request * req;
    if (op == FFSEND) req = &(op->send.transport.mpireq);
    else req = &(op->recv.transport.mpireq);

    res = MPI_Test(req, &flag, MPI_STATUS_IGNORE);
    if (res!=MPI_SUCCESS) return FFERROR;

    *_flag = flag;
    if (flag) return ffop_mpi_progresser_release(op);
    return FFSUCCESS;
}





