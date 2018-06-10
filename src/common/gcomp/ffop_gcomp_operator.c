#include "ff.h"
#include "ffinternal.h"
#include "ffop_gcomp_operator.h"
#include "utils/ffarman.h"

#define SUM(TYPE, A, B, C, SIZE) \
        for (uint32_t i = 0; i<SIZE; i++){ ((TYPE *)C)[i] = ((TYPE *)A)[i] + ((TYPE *)B)[i]; }

static ffop_gcomp_operator_t operators[FFOPERATOR_SENTINEL];

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
            SUM(double, a, b, c, size);
            break;
        default:
            FFLOG_ERROR("Operator not found!\n");
            return FFINVALID_ARG;         
    }

    return FFSUCCESS;
}

int ffop_gcomp_operator_init(){

    operators[FFSUM].type = FFSUM;
    operators[FFSUM].commutative = 1;
    operators[FFSUM].idx = FFSUM;
    operators[FFSUM].op_fun = ffop_gcomp_operator_sum;
   
    return FFSUCCESS;
}

int ffop_gcomp_operator_finalize(){
    return FFSUCCESS;
}

int ffop_gcomp_operator_get(ffoperator_h opidx, ffop_gcomp_operator_t * opdescr){
    
    if (opidx<0 || opidx >= FFOPERATOR_SENTINEL) return FFINVALID_ARG;

    /* copy the descriptor */
    *opdescr = operators[opidx]; 
    return FFSUCCESS;
}

/*
int ffop_gcomp_operator_custom_create(ffoperator_fun_t fun, int commutative, ffoperator_h * handle){

    uint32_t idx = ffarman_get(&custom_op_idx);

    if (idx<0){
        FFLOG_ERROR("Too many custom operators! (check FFCUSTOM_OP_MAX)");
        return FFENOMEM;
    }

    custom_operators[idx].type = FFCUSTOM;
    custom_operators[idx].idx = idx;
    custom_operators[idx].commutative = commutative;
    custom_operators[idx].op_fun = fun;

    *handle = idx;

    return FFSUCCESS;
}

int ffop_gcomp_operator_custom_delete(ffoperator_h opidx){
    
    opidx = opidx - FFCUSTOM;
    if (opidx >= FFCUSTOM_OP_MAX || custom_operators[opidx].type == FFNONE) return FFERROR;
    custom_operators[opidx].type = FFNONE;   

    ffarman_put(&custom_op_idx, opidx);
    
    return FFSUCCESS;
}

*/


