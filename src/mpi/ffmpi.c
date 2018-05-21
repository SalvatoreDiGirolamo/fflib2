
#include "ff.h"
#include "ffinternal.h"
#include "ffmpi.h"
#include "ffop_mpi_progresser.h"

#include <mpi.h>

static int init_by_me = 0;

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
    
    return ffop_mpi_progresser_init();
}

int ffmpi_finalize(){
    if (init_by_me) MPI_Finalize();
    return FFSUCCESS;
}

int ffmpi_get_rank(int * rank){
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
}

int ffmpi_get_size(int * size){
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

