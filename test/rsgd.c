#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "ff.h"

#define FFCALL(X)                       \
    {                                   \
        int ret;                        \
        if (ret = (X) != FFSUCCESS)     \
        {                               \
            printf("Error: %i\n", ret); \
            exit(-1);                   \
        }                               \
    }
extern int num_threads;
void print_array(int32_t *array, int len)
{
    printf("array [ ");
    for (int i = 0; i < len; i++)
    {
        printf("%i ", array[i]);
    }
    printf("]\n");
}
#define N 256
int main(int argc, char *argv[])
{

    unsigned int seed = 6545343;
    int rank, size, count, iters;
    float slowp;
    if (argc != 4)
    {
        printf("Usage: %s <count> <iters> <percentage slow processes [0..1]>\n", argv[0]);
        exit(1);
    }
    int16_t tag = 0;
    count = atoi(argv[1]);
    iters = atoi(argv[2]);
    slowp = atof(argv[3]);
    int32_t *to_reduce[N];
    int32_t *reduced[N];
    for (int i = 0; i < N; i++)
    {
        to_reduce[i] = (int32_t *)calloc(count, sizeof(int32_t));
        reduced[i] = (int32_t *)calloc(count, sizeof(int32_t));
    }
    //int32_t * to_reduce = (int32_t *) calloc(count, sizeof(int32_t));
    //int32_t * reduced = (int32_t *) calloc(count, sizeof(int32_t));

    ffinit(&argc, &argv);
    ffrank(&rank);
    ffsize(&size);

    int to_slow_down = size * slowp;

    printf("Slowing down at most %i processes at each iteration\n", to_slow_down);

    srand(time(NULL) / (rank + 1));
    
    double begin, elapse;
    ffschedule_h solo_allreduce_sched[N];
    ffop_h solo_probe[N];

    for (int i = 0; i < N; i++)
    {
    //ffallreduce(to_reduce[i], reduced[i], count, tag++, FFSUM, FFINT32, 0, &(solo_allreduce_sched[i]));  
        //printf("Threads number = %d\n", num_threads); 
        ffrand_allreduce(to_reduce[i], reduced[i], count, tag++, FFSUM, FFINT32, 0, seed, &(solo_allreduce_sched[i]));
    }



    //ffsolo_allreduce(to_reduce, reduced, count, 0, FFSUM, FFINT32, 0, 20, &solo_allreduce_sched);

    /*if (rank == 0)
    {
        FILE *fp = fopen("test_vis", "w+");
        for (int i = 0; i < N; i++)
        {
            ffschedule_print(solo_allreduce_sched[i], fp, "solo_allreduce");
        }
        fclose(fp);
    }*/


    MPI_Barrier(MPI_COMM_WORLD);
    for (int j = 0; j < N; j++)
    {
        ffschedule_start(solo_allreduce_sched[j]);
    }

    for (int i = 0; i < iters; i++)
    {
        begin = MPI_Wtime();
        for (int j = 0; j < N; j++)
        {

            
            FFLOG("Iteration %i; allreduce: %i\n", i, j);

            for (int k = 0; k < count; k++)
            {
                to_reduce[j][k]++;
            }

            for (int n = 0; n < to_slow_down; n++)
            {
                if (rank == rand() % size)
                {
                    usleep(10000 * (rand() / RAND_MAX));
                    //usleep(100000 * (rand() % 10));
                }
            }

            //memcpy to input buffer

            ffschedule_post(solo_allreduce_sched[j]);
            ffschedule_wait(solo_allreduce_sched[j]);
/*
            int i_activated_it;
            ffop_test(solo_probe[j], &i_activated_it);

            if (!i_activated_it){
                //sum your contribution up
            }
            */
            //memcpy to output buffer

            //print_array(reduced[j], count);
        }
    elapse = MPI_Wtime() - begin;
        if(rank==0)
      printf("Thread = %d, iteration = %d, Total time = %f s\n", rank, i, elapse);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    FFLOG("Completed\n");

    fffinalize();

    for (int i = 0; i < N; i++)
    {
        free(reduced[i]);
        free(to_reduce[i]);
    }
    return 0;
}
