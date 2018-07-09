#ifndef _FFOP_MEM_H_
#define _FFOP_MEM_H_

#include "ff.h"
#include "ffinternal.h"

#define FFOP_MEM_PTR 0x2 
#define FFOP_MEM_IDX 0x4

#define MAX_BUFFER_COUNT 3

#define CHECKBUFFER(BUFFDESCR, MEM) { \
    if (BUFFDESCR.type == FFOP_MEM_IDX && (mem==NULL || \
            BUFFDESCR.idx > MEM->length)){ \
        FFLOG_ERROR("Invalid argument!"); \
        return FFINVALID_ARG; \
    } \
}

#define GETBUFFER(BUFFDESCR, MEM, BUFF) { \
    if (BUFFDESCR.type == FFOP_MEM_PTR){ \
        BUFF = BUFFDESCR.ptr; \
    }else{ \
        BUFF = MEM->buffers[BUFFDESCR.idx]; \
    } \
}


typedef uint32_t ffop_mem_type_t;

typedef struct ffop_mem{
    ffop_mem_type_t type;
    FFLOCK_TYPE lock; 
   
    union{
        void * ptr;
        uint32_t idx;
    };

    uint32_t count;
    ffdatatype_t datatype;

} ffop_mem_t;

typedef struct ffop_mem_set{
    /* ugly */
    void * buffers[MAX_BUFFER_COUNT];
    uint32_t length;

    struct ffmem_set * next;
} ffop_mem_set_t;

int ffop_mem_init(ffop_mem_t * mem);
int ffop_mem_finalize(ffop_mem_t * mem);

#endif /* _FFOP_MEM_H_ */
