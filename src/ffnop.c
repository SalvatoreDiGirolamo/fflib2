#include "ffnop.h"
#include "ffop.h"

int ffnop(ffop_h * _op){
    ffop_t * op;
    ffop_create(&op);
    *_op = (ffop_h) op;

    op->type = FFNOP;

    return FFSUCCESS;
}

int ffnop_post(ffop_t * op, ffop_mem_set_t * mem){
    FFOP_COMPLETED(op);
    return FFSUCCESS;
}

