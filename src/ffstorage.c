

static ffpool_t pools[MAX_POOLS];
static uint32_t next_free_pool;

#define GET(addr, i) ((ffmem_block_t) (addr + i*sizeof(ffmem_block_t)))


int alloc_pool_internal(uint32_t poolid, size_t elem_size, uint32_t count){

    pools[poolid].head = realloc(pools[poolid].head, (sizeof(ffmem_block_t) + elem_size)*count);
    
    for (uint32_t i=pools[poolid].size; i<count - 1; i++){
        GET(pools[poolid], i).next = GET(pools[poolid].head, i+1);
        GET(pools[poolid], i).poolid = poolid;
    }

    GET(pools[poolid], count-1).next = NULL;
    GET(pools[poolid], count-1).poolid = poolid;
    pools[poolid].current_size = count;

    return FF_OK;
}

pool_h ffstorage_pool_create(size_t elem_size, uint32_t initial_count){
    if (next_free_pool >= MAX_POOL) return FF_NOMEM;
    pool_t poolid = next_free_pool++; 
    pools[poolid].pool_size = initial_count;
    pools[poolid].head = NULL;

    if (alloc_pool_internal(poolid, elem_size, initial_count)!=FF_OK) {
        return FF_NOMEM;
    }

    return poolid;
}

int ffstorage_pool_get(pool_h poolid, void ** ptr){
    
    if (pools[poolid].head==NULL){
        /* allocate again */
        pools[poolid].pool_size *= 2;
        if (alloc_pool_internal(poolid, elem_size, pools[poolid].pool_size)!=FF_OK){
            return FF_NOMEM;
        }
    }

    *ptr = pools[poolid].head->address;
    pools[poolid].head = pools[poolid].head->next;
    pools[poolid].curr_size--;

    return FF_OK;
}

int ffstorage_pool_put(void * ptr){
    /* assumes that ptr is the first element of the struct */
    ff_mem_block_t * tofree = (ff_mem_block_t *) ptr;    
    
    tofree->next = pools[tofree->poolid].head;
    pools[tofree->poolid].head = tofree;
    pools[tofree->poolid].curr_size++;
    
    /* TODO: release memory if the pool is almost empty */

    return FF_OK;
}

