
#include "ff.h"
#include "ffinternal.h"
#include "ffmpi.h"
#include "ffop_mpi_progresser.h"

#include "common/gcomp/ffgcomp.h"
#include "common/gcomp/ffop_gcomp.h"

#include <mpi.h>

static int init_by_me = 0;

int ffmpi_bind(ffdescr_t * ffdescr){

    ffdescr->impl.init = ffmpi_init;
    ffdescr->impl.finalize = ffmpi_finalize;
    ffdescr->impl.get_rank = ffmpi_get_rank;
    ffdescr->impl.get_size = ffmpi_get_size;
    ffdescr->impl.register_op = ffmpi_register_op;

    ffdescr->impl.ops[FFSEND].init = ffop_mpi_init;
    ffdescr->impl.ops[FFSEND].post = ffop_mpi_send_post;

    ffdescr->impl.ops[FFRECV].init = ffop_mpi_init;
    ffdescr->impl.ops[FFRECV].post = ffop_mpi_recv_post;

    ffdescr->impl.ops[FFCOMP].init = ffop_gcomp_init;
    ffdescr->impl.ops[FFCOMP].post = ffop_gcomp_post;

    return FFSUCCESS;
}

int ffmpi_init(int * argc, char *** argv){
    int init;
    int mt_level;
    MPI_Initialized(&init);

    if (!init){
        init_by_me = 1;
        
        MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &mt_level);

        if (mt_level!=MPI_THREAD_MULTIPLE){
            FFLOG_ERROR("No MPI_THREAD_MULTIPLE available!\n");
            return FFERROR;
        }
    }
 
    //initialize the generic computation (gcomp) component 
    ffgcomp_init();
   
    return FFSUCCESS; 
}

int ffmpi_register_op(int op, ffop_descriptor_t * descr){
    if (op==FFSEND){
        descr->init = ffop_mpi_init;
        descr->post = ffop_mpi_send_post;
    }else if (op==FFRECV){
        descr->init = ffop_mpi_init;
        descr->post = ffop_mpi_recv_post;
    }else if (op==FFCOMP){
        descr->init = ffop_gcomp_init;
        descr->post = ffop_gcomp_post;
    }else return FFINVALID_ARG;
    
    return FFSUCCESS;
}

int ffmpi_finalize(){
    if (init_by_me) MPI_Finalize();
    ffgcomp_finalize();
    return FFSUCCESS;
}

int ffmpi_get_rank(int * rank){
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    return FFSUCCESS;
}

int ffmpi_get_size(int * size){
    MPI_Comm_size(MPI_COMM_WORLD, size);
    return FFSUCCESS;
}

