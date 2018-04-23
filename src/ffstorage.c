
/* this never releases memory :( */

static ffpool_t pools[MAX_POOLS];
static uint32_t next_free_pool;

#define GET(addr, i) ((ffmem_block_t) (addr + i*sizeof(ffmem_block_t)))

int alloc_pool(uint32_t poolid, size_t elem_size, uint32_t count){
    pools[poolid].head = malloc((sizeof(ffmem_block_t) + elem_size)*count);
    if (pools[poolid].head == NULL) return FF_NOMEM;

    for (uint32_t i=0; i<count - 1; i++){
        GET(pools[poolid].head, i).next = GET(pools[poolid].head, i+1);
        GET(pools[poolid].head, i).poolid = poolid;
    }
    GET(pools[poolid], count-1).head = NULL;
    GET(pools[poolid], count-1).poolid = poolid;

    return FF_OK;
}

pool_h ffstorage_pool_create(size_t elem_size, uint32_t initial_count){
    if (next_free_pool >= MAX_POOL) return FF_NOMEM;
    pool_t poolid = next_free_pool++; 
    pools[poolid].increment = initial_count;

    if (alloc_pool(poolid, elem_size, initial_count)!=FF_OK) return FF_NOMEM;

    return poolid;
}

int ffstorage_pool_get(pool_h poolid, void ** ptr){
    
    if (pools[poolid].head==NULL){
        /* allocate again */
        pools[poolid].increment *= 2;
        if (alloc_pool(poolid, elem_size, pools[poolid])!=FF_OK) return FF_NOMEM;
    }

    *ptr = pools[poolid].address;
    pools[poolid].head = pools[poolid].head->next;

    return FF_OK;
}

int ffstorage_pool_put(void * ptr){
    /* assumes that ptr is the first element of the struct */
    ff_mem_block_t * tofree = (ff_mem_block_t *) ptr;    
    
    tofree->next = pools[tofree->poolid].head;
    pools[tofree->poolid].head = tofree;
    
    return FF_OK;
}

