#include "ffbuffer.h"
#include "ffstorage.h"

#define INITIAL_FFBUFFER_POOL_COUNT 1024

static pool_h buff_pool;

int ffbuffer_create(void * addr, uint32_t count, ffdatatype_h datatype, int options, ffbuffer_h * _ffbuff){

    uint8_t selfalloc = addr==NULL;

    if (selfalloc){
        size_t unitsize;
        ffdatatype_size(datatype, &unitsize);
        addr = malloc(unitsize*count);
        if (addr==NULL) {
            return FFENOMEM;
        }
    }

    ffstorage_pool_get(buff_pool, (void **) _ffbuff);
    ffbuffer_t * ffbuff = *((ffbuffer_t **) _ffbuff);    


    ffbuff->selfalloc = selfalloc;

    if ((options & FFBUFFER_IDX) == FFBUFFER_IDX){
        FFLOG("Creating IDX buffer!\n");
        ffbuff->type = FFBUFFER_IDX;
        ffbuff->idx  = *((uint32_t *) addr);
    }else{
        FFLOG("Creating PTR buffer!\n");
        ffbuff->type = FFBUFFER_PTR;
        ffbuff->ptr = addr;
    }

    ffbuff->count = count;
    ffbuff->datatype = datatype;

    FFLOCK_INIT(&(ffbuff->lock));   
 
    return FFSUCCESS;
}

int ffbuffer_delete(ffbuffer_h _ffbuff){
    ffbuffer_t * ffbuff = (ffbuffer_t *) _ffbuff;    
    FFLOCK_DESTROY(&(ffbuff->lock));
    if (ffbuff->selfalloc) free(ffbuff->ptr);
    return ffstorage_pool_put(ffbuff);
}

/*
int ffbuffer_buffer_init(ffbuffer_t * mem){
    FFLOCK_INIT(&(mem->lock));   
    return FFSUCCESS;
}

int ffbuffer_buffer_finalize(ffbuffer_t * mem){
    FFLOCK_DESTROY(&(mem->lock));
    return FFSUCCESS;
}
*/
int ffbuffer_init(){
    buff_pool = ffstorage_pool_create(sizeof(ffbuffer_t), INITIAL_FFBUFFER_POOL_COUNT);
    return FFSUCCESS;
}

int ffbuffer_finalize(){
    return ffstorage_pool_destroy(buff_pool);
}


