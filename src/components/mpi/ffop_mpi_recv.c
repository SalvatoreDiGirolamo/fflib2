
#include "ffop_mpi.h"
#include "ffop_mpi_progresser.h"
#include "ffrecv.h"

int ffop_mpi_recv_execute(ffop_t * op, ffbuffer_set_t * mem){
    int res;

    ffrecv_t * recv = &(op->recv);

#ifdef CHECK_ARGS
    if (op==NULL || op!=FFRECV) return FFINVALID_ARG;
    
    CHECKBUFFER(recv->buffer, mem); 
   
#endif

    void * buffer;
    GETBUFFER(recv->buffer, mem, buffer);

    uint32_t tag = (recv->tag << 16) | op->version;
    FFLOG("MPI_Irecv count: %u; datatype: %u; source: %u; user tag: %hu; real tag: %u; buffer: %p\n", recv->buffer->count, recv->buffer->datatype, recv->peer, recv->tag, tag, buffer);
    res = MPI_Irecv(buffer, recv->buffer->count, 
            datatype_translation_table[recv->buffer->datatype], recv->peer, 
            tag, MPI_COMM_WORLD, &(recv->transport.mpireq));

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

int ffop_mpi_recv_cancel(ffop_t * op){
    ffrecv_t * recv = &(op->recv);
    FFLOG("Cancelling MPI_Irecv (op: %lu)!\n", op->id);
    MPI_Cancel(&(recv->transport.mpireq));

    //a cancelled op needs to be waited for
    MPI_Wait(&(recv->transport.mpireq), MPI_STATUS_IGNORE); 

    return FFSUCCESS;
}
