
#include "ffop_mpi.h"
#include "ffop_mpi_progresser.h"
#include "ffsend.h"

int ffop_mpi_send_post(ffop_t * op, ffop_mem_set_t * mem){
    int res;

    ffsend_t * send = &(op->send);

#ifdef CHECK_ARGS
    if (op==NULL || op->type!=FFSEND) {
        FFLOG_ERROR("Invalid argument!");
        return FFINVALID_ARG;
    }

    if (send->buffer.type == FFOP_MEM_IDX && (mem==NULL ||
            send->buffer.idx > mem->length)){
        FFLOG_ERROR("Invalid argument!");
        return FFINVALID_ARG;
    }
#endif

    void * buffer;
    if (send->buffer.type == FFOP_MEM_PTR){
        buffer = send->buffer.ptr;
    }else{
        buffer = mem->buffers[send->buffer.idx];
    }

    res = MPI_Isend(buffer, send->buffer.count, 
            datatype_translation_table[send->buffer.datatype], send->peer, 
            send->tag, MPI_COMM_WORLD, &(send->transport.mpireq));

    if (res!=MPI_SUCCESS) {
        FFLOG_ERROR("Error while creating the MPI_Isend!");
        return FFERROR;
    }

    return ffop_mpi_progresser_track(op, send->transport.mpireq);
}

