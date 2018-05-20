
#include "ffop_mpi_progresser.h"
#include "../utils/ffarman.h"
#include "../utils/fflock.h"

static ffop_t *    posted_ops[FFMPI_MAX_REQ];
static MPI_Request requests[FFMPI_MAX_REQ];
static ffarman_t   index_manager;

static fflock_t progress_lock;

int ffop_mpi_progresser_init(){

    FFLOCK_INIT(&progress_lock);
    if (ffarman_create(FFMPI_MAX_REQ, &index_manager)!=FFSUCCESS){
        return FFERROR;
    }
    
    return FFSUCCESS;
}

int ffop_mpi_progresser_finalize(){
    farman_free(index_manager);
    FFLOCK_FREE(&progress_lock);
    return FFSUCCESS;
}

int ffop_mpi_progresser_track(ffop_t * op, MPI_Request req){

    uint32_t idx = ffarman_get(index_manager);

    if (idx<0){
        FFLOG_ERROR("Too many in-flight MPI operations! (check FFMPI_MAX_REQ)");
        return FFENOMEM;
    }

    requests[idx] = req;
    op->transport.idx = idx;
    posted_ops[idx] = op;
    
    return FFSUCCESS;
}

int ffop_mpi_progresser_release(ffop_t * op){

    uint32_t idx = op->transport.idx;    
    ffarman_put(index_manager, idx);
    
    requests[req->idx] = MPI_REQUEST_NULL;
    posted_ops[req->idx] = NULL;

    return FFSUCCESS;
}



int ffop_mpi_progresser_progress(ffop_t ** ready_list){

    int outcount, res;
    int ready_indices[FFMPI_MAX_REQ];
    res = MPI_Testsome(FFMPI_MAX_REQ, requests, &outcount, ready_indices, MPI_STATUS_IGNORE);

    if (res!=MPI_SUCCESS) {
        FFLOG_ERROR("MPI_Testsome returned with error!");
        return FFERROR;
    }
    
    for (int i=0; i<outcount; i++){
        ffop_t * readyop = postedops[ready_indices[i]];

        readyop->next = *ready_list;
        *ready_list = readyop;
        ffop_mpi_progresser_release(readyop);
    }
    
    return FFSUCCESS;
}


