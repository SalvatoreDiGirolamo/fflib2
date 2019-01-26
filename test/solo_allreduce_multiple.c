#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "ff.h"

#define FFCALL(X) { int ret; if (ret=(X)!=FFSUCCESS) { printf("Error: %i\n", ret); exit(-1); } }

void print_array(int32_t * array, int len){
    printf("array [ ");
    for (int i=0; i<len; i++){
        printf("%i ", array[i]);
    }
    printf("]\n");
}

int main(int argc, char * argv[]){
    
    int rank, size, count, iters;
    if (argc!=3){
        printf("Usage: %s <count> <iters>\n", argv[0]);
        exit(1);
    } 

    count = atoi(argv[1]);
    iters = atoi(argv[2]);
    int32_t * to_reduce = (int32_t *) calloc(count, sizeof(int32_t));
    int32_t * reduced = (int32_t *) calloc(count, sizeof(int32_t));

    ffinit(&argc, &argv);
    ffrank(&rank);
    ffsize(&size);

    srand(time(NULL)/(rank+1));
   
    ffschedule_h solo_allreduce_sched;
    ffsolo_allreduce(to_reduce, reduced, count, 0, FFSUM, FFINT32, 0, 4, &solo_allreduce_sched);

    if (rank==0){
        FILE * fp = fopen("test_vis", "w+");
        ffschedule_print(solo_allreduce_sched, fp, "solo_allreduce");
        fclose(fp);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    ffschedule_start(solo_allreduce_sched);

    for(int i=0; i<iters; i++){
        FFLOG("Iteration %i\n", i);
        for (int i=0; i<count; i++){
            to_reduce[i]++;
        }

        if (rank != rand() % size){
            usleep(1000*(rand()/RAND_MAX));
        }

        ffschedule_post(solo_allreduce_sched);
        ffschedule_wait(solo_allreduce_sched);
        print_array(reduced, count);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    FFLOG("Completed\n");

    fffinalize();    
    free(reduced);
    free(to_reduce);
    
    return 0;
}