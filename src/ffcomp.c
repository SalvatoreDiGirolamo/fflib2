
#include "ff.h"
#include "ffop.h"
#include "ffinternal.h"
#include "ffcomp.h"

int ffcomp(void * buff1, void * buff2, int count, ffdatatype_h datatype, ffoperator_h ffoperator, int options, void * buff3, ffop_h * ophandle){

    int res;
    ffop_t * op;
    ffop_create(&op);

    FFLOG("Creating computation\n");
    *ophandle = (ffop_h) op;    

    op->type = FFCOMP;
    op->options &= options; 
       
    op->comp.buffer1.count = count;
    op->comp.buffer1.datatype = datatype;
    
    op->comp.buffer2.count = count;
    op->comp.buffer2.datatype = datatype;

    op->comp.buffer3.count = count;
    op->comp.buffer3.datatype = datatype;

    if (options & FFOP_MEM_IDX == FFOP_MEM_IDX){
        op->comp.buffer1.type = FFOP_MEM_IDX;
        op->comp.buffer1.idx = *((uint32_t *) buff1);

        op->comp.buffer2.type = FFOP_MEM_IDX;
        op->comp.buffer2.idx = *((uint32_t *) buff2);

        op->comp.buffer3.type = FFOP_MEM_IDX;
        op->comp.buffer3.idx = *((uint32_t *) buff3);

    }else{
        op->comp.buffer1.type = FFOP_MEM_PTR;
        op->comp.buffer1.ptr = buff1;

        op->comp.buffer2.type = FFOP_MEM_PTR;
        op->comp.buffer2.ptr = buff2;

        op->comp.buffer3.type = FFOP_MEM_PTR;
        op->comp.buffer3.ptr = buff3;
    }

    op->comp.operator_type = ffoperator;
    return ff.impl.ops[FFCOMP].init(op);
    
}

int ffcomp_operator_create(ffoperator_fun_t fun, int commutative, ffoperator_h * handle){
    return ff.impl.operator_create(fun, commutative, handle);
}

int ffcomp_operator_delete(ffoperator_h handle){
    return ff.impl.operator_delete(handle);
}

