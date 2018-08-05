#include "ffbuffer.h"


int ffbuffer_init(ffbuffer_t * mem){
    FFLOCK_INIT(&(mem->lock));   
    return FFSUCCESS;
}


int ffbuffer_finalize(ffbuffer_t * mem){
    FFLOCK_DESTROY(&(mem->lock));
    return FFSUCCESS;
}


