#ifndef _FFSEND_H_
#define _FFSEND_H_

#include "ffinternal.h"
#include "ffop_mem.h"
#include "mpi/ffop_mpi.h"

typedef struct ffsend{    
    ffpeer_t peer;
    ffop_mem_t buffer;
    uint32_t tag;
    uint8_t flags;

    /* transport */
#ifdef FFOP_MPI
    mpi_transport_t transport;
#endif

} ffsend_t;

int ffsend_post(ffop_t * op, ffop_mem_set_t * mem);
int ffsend_create(void * buffer, size_t size, int source, int tag, int options, ffop_t * io);

#endif /* _FFSEND_H_ */
