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

    ffschedule_h solo_allreduce_sched[N];
    for (int i = 0; i < N; i++)
    {
        ffsolo_allreduce(to_reduce[i], reduced[i], count, tag++, FFSUM, FFINT32, 0, 20, &(solo_allreduce_sched[i]));
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
                    usleep(1000 * (rand() / RAND_MAX));
                }
            }

            ffschedule_post(solo_allreduce_sched[j]);
            ffschedule_wait(solo_allreduce_sched[j]);
            //print_array(reduced[j], count);
        }
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