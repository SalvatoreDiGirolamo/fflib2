
#include "ffstorage.h"
#include "utils/ffarman.h"

static ffpool_t pools[MAX_POOLS];
static uint32_t next_free_pool;
static ffarman_t index_manager;

#define GET(pool, i) ((ffmem_block_t *) (((void *) pool.head) + i*(sizeof(ffmem_block_t) + elem_size)))


int ffstorage_init(){
    ffarman_create(MAX_POOLS, &index_manager);
    return FFSUCCESS;
}

int ffstorage_finalize(){
    ffarman_free(&index_manager);
    return FFSUCCESS;
}

int alloc_pool_internal(uint32_t poolid, size_t elem_size, uint32_t count){

    pools[poolid].head = realloc(pools[poolid].head, (sizeof(ffmem_block_t) + elem_size)*count); 
    pools[poolid].tofree = pools[poolid].head;

    for (uint32_t i=pools[poolid].curr_size; i<count - 1; i++){

        GET(pools[poolid], i)->next = GET(pools[poolid], i+1);
        GET(pools[poolid], i)->poolid = poolid;
        GET(pools[poolid], i)->id = i;
    }

    GET(pools[poolid], count-1)->next = NULL;
    GET(pools[poolid], count-1)->poolid = poolid;
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


    if (alloc_pool_internal(poolid, elem_size, initial_count)!=FFSUCCESS) {
        return FFENOMEM;
    }

    return poolid;
}

int ffstorage_pool_destroy(pool_h poolid){
    free(pools[poolid].tofree); 
    ffarman_put(&index_manager, poolid);
    return FFSUCCESS;
}

int ffstorage_pool_get(pool_h poolid, void ** ptr){
    
    if (pools[poolid].head==NULL){
        /* allocate again */
        if (alloc_pool_internal(poolid, pools[poolid].elem_size, pools[poolid].pool_size*2)!=FFSUCCESS){
            return FFENOMEM;
        }
    }

    *ptr = (void *) pools[poolid].head + pools[poolid].elem_size;
    pools[poolid].head = pools[poolid].head->next;
    pools[poolid].curr_size--;

    return FFSUCCESS;
}

int ffstorage_pool_put(void * ptr){
    /* assumes that ptr is the first element of the struct */
    ffmem_block_t * tofree = (ffmem_block_t *) ptr;    
    
    tofree->next = pools[tofree->poolid].head;
    pools[tofree->poolid].head = tofree;
    pools[tofree->poolid].curr_size++;
    
    /* TODO: release memory if the pool is almost empty */

    return FFSUCCESS;
}

