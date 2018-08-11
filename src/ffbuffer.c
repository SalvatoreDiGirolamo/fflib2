#include "ffbuffer.h"
#include "ffstorage.h"

#include <assert.h>

#define INITIAL_FFBUFFER_POOL_COUNT 1024

static pool_h buff_pool;

int ffbuffer_create(void * addr, uint32_t count, ffdatatype_h datatype, int options, ffbuffer_h * _ffbuff){

    uint8_t selfalloc = addr==NULL;

    ffstorage_pool_get(buff_pool, (void **) _ffbuff);
    ffbuffer_t * ffbuff = *((ffbuffer_t **) _ffbuff);    

    ffbuff->options = options;
    ffbuff->selfalloc = selfalloc;

    if ((options & FFBUFFER_IDX) == FFBUFFER_IDX){  
        assert(!selfalloc);
        FFLOG("Creating IDX buffer!\n");
        ffbuff->type = FFBUFFER_IDX;
        ffbuff->idx  = *((uint32_t *) addr);
    }else{ 
        FFLOG("Creating PTR buffer!\n");
        ffbuff->type = FFBUFFER_PTR;
        ffbuff->ptr = addr;

        if (ffbuff->ptr==NULL && ffbuffer_resize(*_ffbuff, count, datatype) != FFSUCCESS){
            ffstorage_pool_put(_ffbuff);
            return FFENOMEM;
        }
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

int ffbuffer_get_size(ffbuffer_h handle, uint32_t * count, ffdatatype_h * datatype){
    ffbuffer_t * ffbuff = (ffbuffer_t *) handle;    
    *count = ffbuff->count;
    *datatype = ffbuff->datatype;
    return FFSUCCESS;
}

int ffbuffer_resize(ffbuffer_h handle, uint32_t count, ffdatatype_h datatype){
    ffbuffer_t * ffbuff = (ffbuffer_t *) handle;    
    assert((ffbuff->options && FFBUFFER_IDX) != FFBUFFER_IDX);

    size_t unitsize;
    ffdatatype_size(datatype, &unitsize);
    FFLOG("Resizing buffers (addr: %p; size: %lu)\n", ffbuff->ptr, unitsize*count);
    ffbuff->ptr = realloc(ffbuff->ptr, unitsize*count);
    if (ffbuff->ptr==NULL) {
        return FFENOMEM;
    }
    return FFSUCCESS;
}

int ffbuffer_init(){
    buff_pool = ffstorage_pool_create(sizeof(ffbuffer_t), INITIAL_FFBUFFER_POOL_COUNT);
    return FFSUCCESS;
}

int ffbuffer_finalize(){
    return ffstorage_pool_destroy(buff_pool);
}


