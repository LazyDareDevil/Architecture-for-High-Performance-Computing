#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv){
    int rank, size, prev, next;
    int buf[10];
    MPI_Request reqs[20];
    MPI_Status stats[10];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    prev = rank - 1;
    next = rank + 1;
    if(rank == 0){
        for (int i = 1; i < size; ++i){
            MPI_Isend(&rank, 1, MPI_INT, next, 5, MPI_COMM_WORLD, &reqs[i]);
            MPI_Wait(&reqs[i], &stats[i]);
        }
        // MPI_Waitall(size - 1, reqs, stats)
        printf("process %d send %d messages \n", rank, size - 1);
    }
    else{
        MPI_Irecv(&buf[rank], 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &reqs[rank]);
        printf("process %d got '%d' from %d \n", rank, buf[rank], 0);
    }
    
    MPI_Finalize();
    return 0;
}