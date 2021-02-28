#include <iostream>
#include <fstream>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

#define MCW MPI_COMM_WORLD

int main(int argc, char **argv) {

    int rank;
    int size;
    int data;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    if(rank == 0) {

    }
    else {

    }

}
