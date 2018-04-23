#ifndef _FFSTORAGE_H_
#define _FFSTORAGE_H_

/* to avoid using another hashtable here*/
#define MAX_POOLS 128

typedef uint32_t pool_h;


typedef struct ffmem_block{
    void * address; /* DON'T MOVE THIS (see ffstorage_pool_put)! */
    size_t size;
    uint32_t poolid;

    /* used when the block is free */
    struct ffmem_block * next;
} ffmem_block_t;

typedef struct ffpool{
    ff_mem_block_t * head;
    uint32_t increment;
} ffpool_t;


pool_t ffstorage_pool_create(size_t elem_size, uint32_t initial_count);

int ffstorage_pool_get(pool_h pool, void ** ptr);
int ffstorage_pool_put(void * ptr);


#endif /* _FFSTORAGE_H */
