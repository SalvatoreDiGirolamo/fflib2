#include "ffrecv.h"
#include "ffop.h"

int ffrecv(void * buffer, int count, ffdatatype_h datatype, int source, int tag, 
    int options, ffop_h * _op){

    int res; 
    ffop_t * op;
    ffop_create(&op);
    *_op = (ffop_h) op;

    op->type = FFRECV;
    op->options &= options;

    op->recv.peer = source;
    op->recv.tag = tag;
 
    ffop_mem_init(&(op->recv.buffer));   
    op->recv.buffer.count = count;
    op->recv.buffer.datatype = datatype;

    FFLOG("FFRECV ID: %lu; source: %i; count: %i; datatype: %i; tag: %i; options: %i\n", op->id, source, count, datatype, tag, options);

    if (options & FFOP_MEM_IDX == FFOP_MEM_IDX){
        op->recv.buffer.type = FFOP_MEM_IDX;
        op->recv.buffer.idx = *((uint32_t *) buffer);
    }else{
        op->recv.buffer.type = FFOP_MEM_PTR;
        op->recv.buffer.ptr = buffer;
    }
 
    /* implementation specific */   
    res = ff.impl.ops[FFRECV].init(op);

    return res;
}


int ffrecv_tostring(ffop_t * op, char * str, int len){
    snprintf(str, len, "R.%lu(%i)", op->id, op->recv.peer); 
    return FFSUCCESS;
}


int ffrecv_finalize(ffop_t * op){
    ffop_mem_finalize(&(op->recv.buffer));
    return FFSUCCESS;
}
