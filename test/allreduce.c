#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "ff.h"

#define FFCALL(X) { int ret; if (ret=(X)!=FFSUCCESS) { printf("Error: %i\n", ret); exit(-1); } }

int main(int argc, char * argv[]){
    
    int rank, size, count;

    if (argc!=2){
        printf("Usage: %s <count>\n", argv[0]);
        exit(1);
    } 

    count = atoi(argv[1]);

    ffinit(&argc, &argv);

    ffrank(&rank);
    ffsize(&size);

    int32_t to_reduce = 5;
    int32_t reduced = 0;

    
    ffschedule_h allreduce;
    ffallreduce(&to_reduce, &reduced, 1, 0, FFSUM, FFINT32, &allreduce);
    ffschedule_post(allreduce);

    ffschedule_wait(allreduce);

    printf("Rank %i reduced value: %i\n", rank, reduced);

    ffschedule_delete(allreduce);

    fffinalize();

}
