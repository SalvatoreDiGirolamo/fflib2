
int ffop_mpi_wait(ffop_t * op){
#ifdef CHECK_ARGS
    if (op==NULL) return FFINVALID_ARG;
#endif

    int res = MPI_Wait(&(op->transport.mpireq), MPI_STATUS_IGNORE);
    if (res!=MPI_SUCCCESS) return FFERROR;

    return ffop_mpi_progresser_release(op);
}

int ffop_mpi_test(ffop_t * op, int *_flag){
#ifdef CHECK_ARGS
    if (op==NULL) return FFINVALID_ARG;
#endif

    int flag, res;
    res = MPI_Test(&(op->transport.mpireq), &flag, MPI_STATUS_IGNORE);
    if (res!=MPI_SUCCESS) return FFERROR;

    *_flag = flag;
    if (flag) return ffop_mpi_progresser_release(op);
    return FFSUCCESS;
}



int ffop_mpi_init(ffop_t * op){
    return FSUCCESS;
}



