#include "ffrecv.h"
#include "ffop.h"

int ffrecv(void * buffer, int count, ffdatatype_t datatype, int source, int tag, 
    int options, ffop_h * _op){

    ffop_t * op;
    ffop_create(&op);
    *_op = (ffop_h) op;

    op->type = FFRECV;

    op->recv.peer = source;
    op->recv.tag = tag;
    
    op->recv.buffer.count = count;
    op->recv.buffer.datatype = datatype;

    if (options & FFOP_MEM_IDX == FFOP_MEM_IDX){
        op->recv.buffer.type = FFOP_MEM_IDX;
        op->recv.buffer.idx = *((uint32_t *) buffer);
    }else{
        op->recv.buffer.type = FFOP_MEM_PTR;
        op->recv.buffer.ptr = buffer;
    }
 
    /* implementation specific */   
    ops[FFRECV].init(op);

    return FFSUCCESS;
}

