#ifndef _FFRECV_H_
#define _FFRECV_H_

#include "ff.h"
#include "ffinternal.h"
#include "ffop_mem.h"
#include "ffbind.h"

typedef struct ffrecv{    
    ffpeer_t peer;
    ffop_mem_t buffer;
    uint32_t tag;
    uint8_t flags;

    /* transport */
    ffimpl_recv_data_t transport;

} ffrecv_t;


int ffrecv_post(ffop_t * op, ffop_mem_set_t * mem);

int ffrecv_tostring(ffop_t * op, char * str, int len);

int ffrecv_finalize(ffop_t * op);

#endif /* _FFRECV_H_ */

