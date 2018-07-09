
#include "ffop_gcomp_operator.h"
#include "ffop.h"
#include "ffinternal.h"
#include "fflocks.h"

int ffop_gcomp_init(ffop_t * op){
    return ffop_gcomp_operator_get(op->comp.operator_type, &(op->comp.operator));
}


int ffop_gcomp_post(ffop_t * op, ffop_mem_set_t * mem){
    
    ffcomp_t * comp = &(op->comp);
    
#ifdef CHECK_ARGS
    if (op==NULL || op->type!=FFCOMP) {
        FFLOG_ERROR("Invalid argument!");
        return FFINVALID_ARG;
    }

    CHECKBUFFER(comp->buffer1, mem);
    CHECKBUFFER(comp->buffer2, mem);
    CHECKBUFFER(comp->buffer3, mem);
#endif

    void *buffer1, *buffer2, *buffer3;
    GETBUFFER(comp->buffer1, mem, buffer1);
    GETBUFFER(comp->buffer2, mem, buffer2);
    GETBUFFER(comp->buffer3, mem, buffer3);

    if (IS_OPT_SET(op, FFCOMP_DEST_ATOMIC)){
        FFLOCK_LOCK(&(comp->buffer3.lock));
    }

    uint32_t size = MIN(comp->buffer1.count, comp->buffer2.count, comp->buffer3.count);
    ffdatatype_h datatype = comp->buffer1.datatype; // they are the same

    int res = comp->operator.op_fun(buffer1, buffer2, buffer3, size, datatype);

    if (IS_OPT_SET(op, FFCOMP_DEST_ATOMIC)){
        FFLOCK_UNLOCK(&(comp->buffer3.lock));
    }


    if (res==FFSUCCESS){ FFOP_COMPLETED(op); }   

    return res;
}

