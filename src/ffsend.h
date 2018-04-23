#ifndef _FFSEND_H_
#define _FFSEND_H_

typedef struct ffsend{    
    ffpeer_t peer;
    ffop_mem_t buffer;
    uint32_t tag;
    uint8_t flags;

    /* for now we completely rely on MPI for the comm abstraction */
    MPI_Comm mpicomm;

    /* transport */
#ifdef FFOP_MPI
    mpi_transport_t transport;
#endif

} ffsend_t;

int ffsend_post(ffop_t * op, ffop_mem_set_t * mem);
int ffsend_create(void * buffer, size_t size, int source, int tag, MPI_Comm comm int options, ffop_t * io);



#endif /* _FFSEND_H_ */
