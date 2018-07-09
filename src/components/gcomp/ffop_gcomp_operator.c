#include "ff.h"
#include "ffinternal.h"
#include "ffop_gcomp_operator.h"
#include "utils/ffarman.h"

#include <string.h>

#define SUM(TYPE, A, B, C, SIZE) \
        for (uint32_t i = 0; i<SIZE; i++){ ((TYPE *)C)[i] = ((TYPE *)A)[i] + ((TYPE *)B)[i]; }


static ffop_gcomp_operator_t operators[FFOPERATOR_SENTINEL];
static ffop_gcomp_operator_t custom_operators[FFMAX_CUSTOM_OPERATORS];
static ffarman_t custom_idx;

int ffop_gcomp_operator_sum(void * a, void * b, void* c, uint32_t size, ffdatatype_h type){

    switch (type){
        case FFINT32: 
            SUM(int32_t, a, b, c, size);
            break;
        case FFINT64:
            SUM(int64_t, a, b, c, size);
            break;
        case FFDOUBLE:
            SUM(double, a, b, c, size);
            break;
        case FFFLOAT:
            SUM(float, a, b, c, size);
            break;
        default:
            FFLOG_ERROR("Operator not found!\n");
            return FFINVALID_ARG;         
    }

    return FFSUCCESS;
}


int ffop_gcomp_operator_copy(void * a, void * b, void* c, uint32_t size, ffdatatype_h type){
    size_t unitsize;
    ffdatatype_size(type, &unitsize);

    memcpy(c, a, size*unitsize);

    return FFSUCCESS;
}


int ffop_gcomp_operator_init(){

    operators[FFSUM].type = FFSUM;
    operators[FFSUM].commutative = 1;
    operators[FFSUM].idx = FFSUM;
    operators[FFSUM].op_fun = ffop_gcomp_operator_sum;

    operators[FFIDENTITY].type = FFIDENTITY;
    operators[FFIDENTITY].commutative = 1;
    operators[FFIDENTITY].idx = FFIDENTITY;
    operators[FFIDENTITY].op_fun = ffop_gcomp_operator_copy;


    if (ffarman_create(FFMAX_CUSTOM_OPERATORS, &custom_idx)!=FFSUCCESS){
        return FFERROR;
    }    

    return FFSUCCESS;
}

int ffop_gcomp_operator_finalize(){
    return FFSUCCESS;
}

int ffop_gcomp_operator_get(ffoperator_h opidx, ffop_gcomp_operator_t * opdescr){
    
    if (opidx<0) return FFINVALID_ARG;

    /* copy the descriptor */
    if (opidx < FFOPERATOR_SENTINEL){
        *opdescr = operators[opidx]; 
        return FFSUCCESS;
    }

    opidx = opidx - FFCUSTOM;
    if (opidx < FFMAX_CUSTOM_OPERATORS){
        *opdescr = custom_operators[opidx];
        return FFSUCCESS;
    }

    return FFINVALID_ARG;
}


int ffop_gcomp_operator_custom_create(ffoperator_fun_t fun, int commutative, ffoperator_h * handle){


    uint32_t idx = ffarman_get(&custom_idx);

    if (idx<0){
        FFLOG_ERROR("Too many custom operators! (check FFMAX_CUSTOM_OPERATORS)");
        return FFENOMEM;
    }

    custom_operators[idx].type = FFCUSTOM;
    custom_operators[idx].idx = idx;
    custom_operators[idx].commutative = commutative;
    custom_operators[idx].op_fun = fun;

    *handle = idx + FFCUSTOM;

    return FFSUCCESS;
}

int ffop_gcomp_operator_custom_delete(ffoperator_h opidx){
    
    opidx = opidx - FFCUSTOM;
    if (opidx >= FFMAX_CUSTOM_OPERATORS || custom_operators[opidx].type == FFNONE) return FFERROR;
    custom_operators[opidx].type = FFNONE;   

    ffarman_put(&custom_idx, opidx);
    
    return FFSUCCESS;
}

