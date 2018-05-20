#ifndef _FFRECV_H_
#define _FFRECV_H_

#include "ff.h"
#include "ffinternal.h"
#include "ffop_mem.h"
#include "mpi/ffop_mpi.h"

typedef struct ffrecv{    
    ffpeer_t peer;
    ffop_mem_t buffer;
    uint32_t tag;
    uint8_t flags;

    /* transport */
#ifdef FFOP_MPI
    mpi_transport_t transport;
#endif

} ffrecv_t;


int ffrecv_post(ffop_t * op, ffop_mem_set_t * mem);


#endif /* _FFRECV_H_ */

