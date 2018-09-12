#ifndef _FFQMAN_H_
#define _FFQMAN_H_

#include "ffinternal.h"
#include "fflocks.h"

#define INITIAL_FFQMAN_ENTRY_COUNT 2048

typedef struct ffqman_entry {
    void * ptr;
    struct ffqman_entry * next;
} ffqman_entry_t;

typedef struct ffqman {
    ffqman_entry_t * head;
    ffqman_entry_t * tail;
    FFLOCK_TYPE head_lock;
    FFLOCK_TYPE tail_lock;
} ffqman_t;


int ffqman_init();
int ffqman_finalize();
int ffqman_create(uint32_t count, ffqman_t * queue);
int ffqman_free(ffqman_t * queue);
int ffqman_push(ffqman_t * queue, void * ptr);
int ffqman_pop(ffqman_t * queue, void ** ptr);

#endif /* _FFQMAN_H_ */
