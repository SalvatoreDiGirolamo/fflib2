#include "ffsend.h"
#include "ffop.h"

int ffsend(void * buffer, int count, ffdatatype_h datatype, int source, int tag, 
    int options, ffop_h * _op){
 
    int res;   
    ffop_t * op;
    ffop_create(&op);
    *_op = (ffop_h) op;

    op->type = FFSEND;

    op->send.peer = source;
    op->send.tag = tag;

    op->send.buffer.count = count;
    op->send.buffer.datatype = datatype;

    if (options & FFOP_MEM_IDX == FFOP_MEM_IDX){
        op->send.buffer.type = FFOP_MEM_IDX;
        op->send.buffer.idx = *((uint32_t *) buffer);
    }else{
        op->send.buffer.type = FFOP_MEM_PTR;
        op->send.buffer.ptr = buffer;
    }

    /* implementation specific */
    res = ops[FFSEND].init(op);
    
    return res;
}





