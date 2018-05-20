
#include "../ff.h"
#include "ffmpi.h"

#include <mpi.h>

static int init_by_me = 0;

int ffmpi_init(int * argc, char *** argv){
    int init;
    MPI_Initialized(&init);

    if (!init){
        init_by_me = 1;
        MPI_Init(argc, argv);
    }

    return FFSUCCESS;
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

