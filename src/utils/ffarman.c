
#include "ffarman.h"

#include <assert.h>

int ffarman_create(uint32_t count, ffarman_t * array){

    array->free_entries = (entry_t *) malloc(sizeof(entry_t) * (count+1));
    array->tofree = array->free_entries;

    if (array->free_entries==NULL){
        return FFENOMEM;    
    }

    array->free_free_entries = NULL;

    for (uint32_t i=0; i<count; i++){
        array->free_entries[i].idx = i;
        array->free_entries[i].next = &(array->free_entries[i+1]);
    }
    array->free_entries[count-1].next = NULL;    

    return FFSUCCESS;
}

int ffarman_free(ffarman_t array){
    free(array.tofree);
}

uint32_t ffarman_get(ffarman_t array){   

    entry_t * e = array.free_entries;
    if (e == NULL) return -1;

    /* remove from free_entries */
    array.free_entries = e->next;

    /* add to free_free entries */
    e->next = array.free_free_entries;
    array.free_free_entries = e;

    /* return the index */
    return e->idx;
}

int ffarman_put(ffarman_t array, uint32_t idx){

    /* take an entry that we can use to store the free_entry */
    entry_t * e = array.free_free_entries;
    assert(e!=NULL);
    array.free_free_entries = e->next;    

    e->idx = idx;
    
    /* make the entry available */
    e->next = array.free_entries;
    array.free_entries = e;
    
    return FFSUCCESS;
}


