
#include "ffop_mpi.h"
#include "ffop_mpi_progresser.h"
#include "../ffrecv.h"

int ffop_mpi_recv_post(ffop_t * op, ffop_mem_set_t * mem){
    int res;

    ffrecv_t * recv = &(op->recv);

#ifdef CHECK_ARGS
    if (op==NULL || op!=FFRECV) return FFINVALID_ARG;
    
    if (recv->buffer.type == FFOP_MEM_IDX && (mem==NULL ||
            recv->buffer.idx > mem->length)){
        return FFINVALID_ARG;
    }
#endif

    void * buffer;
    if (recv->buffer.type == FFOP_MEM_PTR){
        buffer = recv->buffer.ptr;
    }else{
        buffer = mem->buffers[recv->buffer.idx];
    }

    res = MPI_Irecv(buffer, recv->buffer.count, 
            datatype_translation_table[recv->buffer.datatype], recv->peer, 
            recv->tag, MPI_COMM_WORLD, &(recv->transport.mpireq));

    if (res!=MPI_SUCCESS) return FFERROR;
    return ffop_mpi_progresser_track(op, recv->transport.mpireq);
}

