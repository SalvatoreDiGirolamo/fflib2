#include "ffsend.h"
#include "ffop.h"

int ffsend(void * buffer, size_t size, int source, int tag, 
    int options, ffop_h * _op){
    
    ffop_t * op = (ffop_t *) _op;

    op->type = FFSEND;
    op->out_dep_count=0;
    op->in_dep_count=0;
    op->next = NULL;

    op->send.peer = source;
    op->send.tag = tag;

    op->send.buffer.size = size;

    if (options & FFOP_MEM_IDX == FFOP_MEM_IDX){
        op->send.buffer.type = FFOP_MEM_IDX;
        op->send.buffer.idx = *((uint32_t *) buffer);
    }else{
        op->send.buffer.type = FFOP_MEM_PTR;
        op->send.buffer.ptr = buffer;
    }

    /* implementation specific */
    ops[FFSEND].init(op);
    
    return FFSUCCESS;
}





