#ifndef _FFOP_LIST_
#define _FFOP_LIST_

typedef struct ffop_set_entry{
    

    struct ffop_set_entry * next;
} ffop_set_entry_t;

typedef struct ffop_set {

    ffop_set_entry_t * head;   
    ffop_set_entry_t * next_free;

    uint32_t capacity;
    uint32_t count;
    void * key_null;

} ffop_set_t;


int ffop_set_create(int initial_count, uint32_t key_offset, size_t key_size, 
        size_t key_null, ffop_set_t * set){
    
    set->next_free = (ffop_set_entry_t *) malloc(sizeof(ffset_entry_t)*initial_count);
    if (set->ops==NULL) return FFENOMEM;

    set->keys = malloc(key_size*initial_count);
    if (set->keys==NULL) {
        free(set->ops);
        return FFENOMEM;
    }
    
    set->count = initial_count;
    set->capacity = initial_count;
    set->key_null = key_null;

    set->head = NULL;    

    for (int i=0; i<initial_count; i++){
        memcpy((void *) (((char *) set->keys) + i*key_size), key_null, key_size);
        ffset_entry_t
    }
    

    return FFSUCCESS;
}

int fflist_add(ffop_t * op){

    int err;
    if (next_free==NULL && (err=extend())!=FSUCCESS) return err;

    assert(next_free!=NULL);

    ffset_entry_t * entry = next_free;
    next_free = next_free->next;
        
    



}

#endif /*_FFOP_LIST_*/
