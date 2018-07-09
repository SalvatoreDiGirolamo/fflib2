#include "ffsend.h"
#include "ffop.h"

int ffsend(void * buffer, int count, ffdatatype_h datatype, int dest, int tag, 
    int options, ffop_h * _op){
 
    int res;   
    ffop_t * op;
    ffop_create(&op);
    *_op = (ffop_h) op;

    op->type = FFSEND;
    op->options &= options;

    op->send.peer = dest;
    op->send.tag = tag;

    ffop_mem_init(&(op->send.buffer));
    op->send.buffer.count = count;
    op->send.buffer.datatype = datatype;

    FFLOG("FFSEND ID: %lu; dest: %i; count: %i; datatype: %i; tag: %i; options: %i\n", op->id, dest, count, datatype, tag, options);

    if (options & FFOP_MEM_IDX == FFOP_MEM_IDX){
        op->send.buffer.type = FFOP_MEM_IDX;
        op->send.buffer.idx = *((uint32_t *) buffer);
    }else{
        op->send.buffer.type = FFOP_MEM_PTR;
        op->send.buffer.ptr = buffer;
    }

    /* implementation specific */
    res = ff.impl.ops[FFSEND].init(op);
    
    return res;
}


int ffsend_tostring(ffop_t * op, char * str, int len){
    snprintf(str, len, "S.%lu(%i)", op->id, op->send.peer); 
    return FFSUCCESS;
}


int ffsend_finalize(ffop_t * op){

    ffop_mem_finalize(&(op->send.buffer));
    return FFSUCCESS;
}
