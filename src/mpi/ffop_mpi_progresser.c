typedef struct mpi_req {
    uint32_t idx;
    struct mpi_req * next;
} mpi_req_t;

static ffop_t *    posted_ops[FFMPI_MAX_REQ];
static MPI_Request requests[FFMPI_MAX_REQ];
static mpi_req_t * free_requests;
static mpi_req_t * busy_requests;

int ffop_mpi_progresser_init(){

    free_requests = (mpi_req_t *) malloc(sizeof(mpi_req_t)*(FFMPI_MAX_REQ+1));

    if (free_requests==NULL) return FFENOMEM;

    for (int i=0; i<FFMPI_MAX_REQ; i++){
        requests[i] = MPI_REQUEST_NULL;
        posted_ops[i] = NULL;
        free_requests[i].idx = i;
        free_requests[i].next = free_requests[i+1];
    }

    free_requests[FFMPI_MAX_REQ-1].next = NULL;
    busy_requests = NULL;
 
    return FFSUCCESS;   
}

int ffop_mpi_progresser_track(ffop_t * op){

    if (free_requests==NULL){
        FFLOG_ERROR("Too many in-flight MPI operations! (check FFMPI_MAX_REQ)");
        return FFENOMEM;
    }
    
    mpi_req_t * req = free_requests;
    free_requests = free_requests->next;
    req->next = busy_requests;
    busy_requests = req;

    requests[req->idx] = op->transport.mpireq;
    posted_ops[req->idx] = op;
    op->transport.idx = req->idx;
    
    return FFSUCCESS;
}

int ffop_mpi_progresser_release(ffop_t * op){
    assert(busy_requests!=NULL);

    /* get an mpi_req_t that is currently not used for tracing the free slots */
    mpi_req_t * req = busy_requests;
    busy_requests = busy_requests->next;

    /* initialize it */
    req->idx = op->transport.idx;    
    requests[req->idx] = MPI_REQUEST_NULL;
    posted_ops[req->idx] = NULL;
    
    /* put it into the free_requests list */
    req->next = free_requests;
    free_requests = req;

    return FFSUCCESS;
}



int ffop_mpi_progresser_progress(ffop_t ** ready_list){

/* int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount, 
                int array_of_indices[], MPI_Status array_of_statuses[])
*/


    /* this is not very good if FFMPI_MAX_REQ is big*/
    int outcount, res;
    int ready_indices[FFMPI_MAX_REQ];
    res = MPI_Testsome(FFMPI_MAX_REQ, requests, &outcount, ready_indices, MPI_STATUS_IGNORE);

    if (res!=MPI_SUCCESS) {
        FFLOG_ERROR("MPI_Testsome returned with error!");
        return FFERROR;
    }
    
    for (int i=0; i<outcount; i++){
        posted_ops[ready_indices[i]]->next = *ready_list;
        *ready_list = posted_ops[ready_indices[i]];
        ffop_mpi_complete(postedops[ready_indices[i]]);
    }
    
    return FFSUCCESS;
}


