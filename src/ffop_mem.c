#include "ffop_mem.h"


int ffop_mem_init(ffop_mem_t * mem){
    FFLOCK_INIT(&(mem->lock));   
    return FFSUCCESS;
}


int ffop_mem_finalize(ffop_mem_t * mem){
    FFLOCK_DESTROY(&(mem->lock));
    return FFSUCCESS;
}


