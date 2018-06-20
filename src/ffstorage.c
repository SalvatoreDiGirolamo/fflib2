
#include "ffstorage.h"
#include "utils/ffarman.h"

static ffpool_t pools[MAX_POOLS];
static uint32_t next_free_pool;
static ffarman_t index_manager;

#define GET(POOL, OFF) ((ffmem_block_t *) (((uint8_t *) POOL.mem) + (OFF)*(sizeof(ffmem_block_t) + elem_size)))

int ffstorage_init(){
    ffarman_create(MAX_POOLS, &index_manager);
    return FFSUCCESS;
}

int ffstorage_finalize(){
    ffarman_free(&index_manager);
    return FFSUCCESS;
}

int alloc_pool_internal(uint32_t poolid, size_t elem_size, uint32_t count){

    pools[poolid].mem = realloc(pools[poolid].mem, (sizeof(ffmem_block_t) + elem_size)*count); 
    pools[poolid].head = pools[poolid].mem;

    //FFLOG("head: %p\n", pools[poolid].head);
    for (uint32_t i=pools[poolid].curr_size; i < count - 1; i++){
        
        GET(pools[poolid], i)->next = GET(pools[poolid], i+1);
        //FFLOG("current: %p; head->next: %p (size: %lu) man: %p\n", GET(pools[poolid], i), GET(pools[poolid], i)->next, elem_size, ((uint8_t *) GET(pools[poolid], i)) + sizeof(ffmem_block_t) + elem_size);
        GET(pools[poolid], i)->poolid = poolid;
        GET(pools[poolid], i)->id = i;
    }

    GET(pools[poolid], count-1)->next = NULL;
    GET(pools[poolid], count-1)->poolid = poolid;
    GET(pools[poolid], count-1)->id = count - 1;

    pools[poolid].curr_size += count - pools[poolid].pool_size;
    pools[poolid].pool_size = count;

    return FFSUCCESS;
}

pool_h ffstorage_pool_create(size_t elem_size, uint32_t initial_count){
    if (next_free_pool >= MAX_POOLS) return FFENOMEM;
    pool_h poolid = ffarman_get(&index_manager);
    pools[poolid].head = NULL;
    pools[poolid].elem_size = elem_size;
    pools[poolid].curr_size = 0;
    pools[poolid].pool_size = 0;

    //printf("creating pool: %u; %p\n", poolid, pools[poolid].head);

    if (alloc_pool_internal(poolid, elem_size, initial_count)!=FFSUCCESS) {
        return FFENOMEM;
    }

    return poolid;
}

int ffstorage_pool_destroy(pool_h poolid){
    free(pools[poolid].mem); 
    ffarman_put(&index_manager, poolid);
    return FFSUCCESS;
}

int ffstorage_pool_get(pool_h poolid, void ** ptr){
    
    //printf("checking pool: %u; %p\n", poolid, pools[poolid].head);
    if (pools[poolid].head==NULL){
        /* allocate again */
        if (alloc_pool_internal(poolid, pools[poolid].elem_size, pools[poolid].pool_size*2)!=FFSUCCESS){
            return FFENOMEM;
        }
    }

    *ptr = (void *) ((uint8_t *) pools[poolid].head + sizeof(ffmem_block_t));
    //printf("new head: %p; size: %lu\n", pools[poolid].head->next, pools[poolid].elem_size);
    pools[poolid].head = pools[poolid].head->next;
    pools[poolid].curr_size--;

    return FFSUCCESS;
}

int ffstorage_pool_put(void * ptr){
    ffmem_block_t * tofree = (ffmem_block_t *) ((uint8_t *) ptr - sizeof(ffmem_block_t));    
    
    tofree->next = pools[tofree->poolid].head;
    pools[tofree->poolid].head = tofree;
    pools[tofree->poolid].curr_size++;
    
    /* TODO: release memory if the pool is almost empty */

    return FFSUCCESS;
}

