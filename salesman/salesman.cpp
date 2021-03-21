#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>


#define MCW MPI_COMM_WORLD

int main(int argc, char **argv) {
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    srand(rank + time(NULL));


    MPI_Finalize();
}