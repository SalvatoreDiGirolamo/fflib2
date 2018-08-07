
#include "ffop_mpi.h"
#include "ffop_mpi_progresser.h"
#include "ffrecv.h"

int ffop_mpi_recv_post(ffop_t * op, ffbuffer_set_t * mem){
    int res;

    ffrecv_t * recv = &(op->recv);

#ifdef CHECK_ARGS
    if (op==NULL || op!=FFRECV) return FFINVALID_ARG;
    
    CHECKBUFFER(recv->buffer, mem); 
   
#endif

    void * buffer;
    GETBUFFER(recv->buffer, mem, buffer);

    FFLOG("MPI_Irecv buffer: %p\n", buffer);
    res = MPI_Irecv(buffer, recv->buffer->count, 
            datatype_translation_table[recv->buffer->datatype], recv->peer, 
            recv->tag, MPI_COMM_WORLD, &(recv->transport.mpireq));

    if (res!=MPI_SUCCESS) return FFERROR;
    return ffop_mpi_progresser_track(op, recv->transport.mpireq);
}

int ffop_mpi_recv_wait(ffop_t * op){
    MPI_Wait(&(op->recv.transport.mpireq), MPI_STATUS_IGNORE);
    return FFSUCCESS;
}

int ffop_mpi_recv_test(ffop_t * op, int * flag){
    MPI_Test(&(op->recv.transport.mpireq), flag, MPI_STATUS_IGNORE);
    return FFSUCCESS;
}


