#include "ffqman.h"
#include "ffstorage.h"

static pool_h qman_entry_pool;

void ffqman_entry_init(void * _entry){
    ffqman_entry_t * entry = (ffqman_entry_t *) _entry;
    FFLOCK_INIT(&(entry->lock));
}

int ffqman_init(){
    qman_entry_pool = ffstorage_pool_create(sizeof(ffqman_entry_t), INITIAL_FFQMAN_ENTRY_COUNT, ffqman_entry_init);
    return FFSUCCESS;
}

int ffqman_finalize(){
    ffstorage_pool_destroy(qman_entry_pool);
    return FFSUCCESS;
}

int ffqman_create(ffqman_t * queue){
    ffstorage_pool_get(qman_entry_pool, (void **) &(queue->head_sentinel));
    ffstorage_pool_get(qman_entry_pool, (void **) &(queue->tail_sentinel));

    queue->head = queue->head_sentinel;
    queue->tail = queue->head_sentinel;

    queue->head->next = queue->tail_sentinel;
    return FFSUCCESS;
}

int ffqman_free(ffqman_t * queue){
    return FFSUCCESS;
}

int ffqman_push(ffqman_t * queue, void * ptr){

    ffqman_entry_t * el;
    ffstorage_pool_get(qman_entry_pool, (void **) &el);

    el->next = NULL;
    el->ptr = ptr;
 
    while (1){
        ffqman_entry_t * tail = queue->tail;  
        ffqman_entry_t * tail_next = queue->tail->next;  

        //FFLOG("pushing %p on %p: locking\n", queue, ptr);
        //FFLOG("locking %i (1)\n", tail->lock);
        FFLOCK_LOCK(&(tail->lock));
        //FFLOG("locking %i (2)\n", tail_next->lock);
        FFLOCK_LOCK(&(tail_next->lock));
        //FFLOG("locked!\n");
    
        if (queue->tail == tail && queue->tail->next == tail_next){
            el->next = queue->tail_sentinel;
            queue->tail->next = el;
            queue->tail = el;

            FFLOCK_UNLOCK(&(tail_next->lock));
            FFLOCK_UNLOCK(&(tail->lock));

            return FFSUCCESS;
        }
        
        FFLOCK_UNLOCK(&(tail_next->lock));
        //FFLOG("unlocked (2)\n");
        FFLOCK_UNLOCK(&(tail->lock));
        //FFLOG("unlocked (1)\n");
    }

    return FFSUCCESS;
}

int ffqman_pop(ffqman_t * queue, void ** ptr){
    
    ffqman_entry_t *el, *head, *head_next;

    //FFLOG("popping from %p\n", queue);

    while (1){
        head = queue->head;
        head_next = queue->head->next;
    
        //FFLOG("locking %i (1)\n", head->lock);
        FFLOCK_LOCK(&(head->lock));
        //FFLOG("locking %i (2)\n", head_next->lock);
        FFLOCK_LOCK(&(head_next->lock));
        //FFLOG("locked!\n");
    
        if (queue->head == head && queue->head->next == head_next){
            if (head_next == queue->tail_sentinel) {
                //FFLOG("queue %p is empty --> returning NULL\n", queue);
                *ptr = NULL;
            }else{
                //FFLOG("queue %p is not EMPTY --> returning %p\n", queue, queue->head->ptr);
                el = head_next;
                *ptr = el->ptr;
                queue->head->next = head_next->next;
                if (head_next == queue->tail){
                    queue->tail = queue->head_sentinel;
                }
                ffstorage_pool_put(el);
            }

            FFLOCK_UNLOCK(&(head_next->lock));
            //FFLOG("unlocked (2)\n");
            FFLOCK_UNLOCK(&(head->lock));
            //FFLOG("unlocked (1)\n");
            return FFSUCCESS;
        }   
    
        FFLOCK_UNLOCK(&(head_next->lock));
        FFLOCK_UNLOCK(&(head->lock));
    }
    
    return FFSUCCESS;
}

