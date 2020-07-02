#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv){
    int rank, size, prev, next;
    int buf[10];
    MPI_Request reqs_recv[10];
    MPI_Request reqs_send[10];
    MPI_Status stats[10];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank==0) prev = size - 1;
    if(rank==size - 1) next = 0;
    for (int i = 0; i < size; ++i){
        if (i != rank){
            next = i;
            prev = i;
            MPI_Sendrecv(&rank, 1, MPI_INT, next, 6, &buf[i], 1, MPI_INT, prev, 6, MPI_COMM_WORLD, &stats[i]);
            printf("process %d got '%d' from %d, send '%d' to %d \n", rank, buf[i], prev, rank, next);
        }   
    }
    MPI_Finalize();
    return 0;
}