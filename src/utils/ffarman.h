
#include "../ffinternal.h"

typedef struct entry {
    uint32_t idx;
    struct entry * next;
} entry_t;

typedef struct ffarman{
    /* entries in data that can be used */
    entry_t * free_entries;

    /* pointer to the memory to be freed */
    entry_t * tofree;

    /* entries that can be reused as free_entries */    
    entry_t * free_free_entries;
} ffarman_t;


int ffarman_create(uint32_t count, ffarman_t * array);
int ffarman_free(ffarman_t array);
uint32_t ffarman_get(ffarman_t array);
int ffarman_put(ffarman_t array, uint32_t idx);


