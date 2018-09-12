#include "ffqman.h"
#include "ffstorage.h"

static pool_h qman_entry_pool;

int ffqman_init(){
    qman_entry_pool = ffstorage_pool_create(sizeof(ffqman_entry_t), INITIAL_FFQMAN_ENTRY_COUNT);
    return FFSUCCESS;
}

int ffqman_finalize(){
    ffstorage_pool_destroy(qman_entry_pool);
    return FFSUCCESS;
}


int ffqman_create(uint32_t count, ffqman_t * queue){
    queue->head = NULL;
    queue->tail = NULL;
    FFLOCK_INIT(&(queue->head_lock));
    FFLOCK_INIT(&(queue->tail_lock));
    return FFSUCCESS;
}

int ffqman_free(ffqman_t * queue){
    FFLOCK_DESTROY(&(queue->head_lock));
    FFLOCK_DESTROY(&(queue->tail_lock));
    return FFSUCCESS;
}

int ffqman_push(ffqman_t * queue, void * ptr){
    int head_locked = 0;

    ffqman_entry_t * el;
    ffstorage_pool_get(qman_entry_pool, (void **) &el);

    el->next = NULL;
    el->ptr = ptr;
    
    // lock tail
    FFLOCK_LOCK(&(queue->tail_lock));
    
    if (queue->tail==NULL || queue->tail==queue->head){
        FFLOCK_LOCK(&(queue->head_lock));
        head_locked = 1;
    }

    if (queue->tail == NULL){
        queue->head = el;
        queue->tail = el;
    }else{
        queue->head->next = el;
        el->next = NULL;
    }

    if (head_locked) { FFLOCK_UNLOCK(&(queue->head_lock)); }
    FFLOCK_UNLOCK(&(queue->tail_lock));

    return FFSUCCESS;
}

int ffqman_pop(ffqman_t * queue, void ** ptr){
    int tail_locked = 0;
    
    ffqman_entry_t * el;

    //lock head
    FFLOCK_LOCK(&(queue->head_lock));

    if (queue->head == NULL || queue->head == queue->tail){
        FFLOCK_LOCK(&(queue->tail_lock));
        tail_locked = 1;
    }
    
    if (queue->head == NULL){
        *ptr = NULL;
    }else{
        el = queue->head->ptr;
        queue->head = queue->head->next;
        *ptr = el->ptr;
        ffstorage_pool_put(el);
    }
    
    if (tail_locked) { FFLOCK_UNLOCK(&(queue->tail_lock)); }
    FFLOCK_UNLOCK(&(queue->head_lock));

    return FFSUCCESS;
}

