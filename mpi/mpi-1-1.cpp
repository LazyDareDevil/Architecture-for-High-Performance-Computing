#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv)
{
   int rank, size, prev, next;
   int buf[2];
   MPI_Request reqs[4];
   MPI_Status stats[4];
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   printf("process size: %d rank: %d\n", size, rank);
   MPI_Finalize();
   return 0;
}
