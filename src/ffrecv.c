#include "ffrecv.h"
#include "ffop.h"

int ffrecv(void * buffer, size_t size, int source, int tag, 
    int options, ffop_h * _op){

    ffop_t * op = (ffop_t *) _op;

    op->type = FFRECV;
    op->out_dep_count=0;
    op->in_dep_count=0;
    op->next = NULL;

    op->recv.peer = source;
    op->recv.tag = tag;
    
    op->recv.buffer.size = size;
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

