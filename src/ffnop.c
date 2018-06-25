#include "ffnop.h"
#include "ffop.h"

int ffnop(int options, ffop_h * _op){
    ffop_t * op;
    ffop_create(&op);
    *_op = (ffop_h) op;
    op->options = options;

    op->type = FFNOP;

    FFLOG("creating ffnop %p\n", op);
    return FFSUCCESS;
}

int ffnop_post(ffop_t * op, ffop_mem_set_t * mem){
    FFOP_COMPLETED(op);
    return FFSUCCESS;
}

