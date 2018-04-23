

int ffop_mpi_send_post(ffop_t * op, ffop_mem_set_t * mem){
    int res;

#ifdef CHECK_ARGS
    if (op==NULL) return FFINVALID_ARG;
    
    if (op->buffer.type == FFOP_MEM_IDX && (mem==NULL ||
            op->buffer.idx > mem->length)){
        return FFINVALID_ARG;
    }
#endif

    void * buffer;
    if (op->buffer.type == FFOP_MEM_PTR){
        buffer = op->buffer.ptr;
    }else{
        buffer = mem->buffers[op->buffer.idx];
    }

    res = MPI_ISend(buffer, op->size, MPI_CHAR, op->peer, op->tag, op->mpicomm, 
        &(op->transport.mpireq));

    if (res!=MPI_SUCCESS) return FFERROR;
    return ffop_mpi_progresser_track(op);

}

