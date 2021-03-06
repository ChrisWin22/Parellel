#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>

#define MCW MPI_COMM_WORLD


// There are 7 rows and columns
// rank 0 is the master

void passTo(int rank, int data) {
    int random = rand()%2;
    int nextLevel = data + 1;


    if(rank == 7) {
        MPI_Send(&nextLevel, 1, MPI_INT, rank - 1, 0, MCW);
    }
    else if(rank == 1) {
        MPI_Send(&nextLevel, 1, MPI_INT, rank + 1, 0, MCW);
    }

    else if(random == 1) { // pass to the right
        if(data % 2 == 0) { // on even row
            MPI_Send(&nextLevel, 1, MPI_INT, rank + 1, 0, MCW);
        }
        else { // on odd row
            MPI_Send(&nextLevel, 1, MPI_INT, rank, 0, MCW);
        }
    }
    else { //pass to left
        if(data % 2 == 0) { //on even row
            MPI_Send(&nextLevel, 1, MPI_INT, rank, 0, MCW);
        }
        else { // on odd row
            MPI_Send(&nextLevel, 1, MPI_INT, rank - 1, 0, MCW);
        }
    }
}


int main(int argc, char **argv) {
    int rank, size;
    int data = 0;
    int finished = 0;

    //Initialize
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    if(rank == 0) {
        srand(time(0));
        int numReturned = 0;
        int numBalls = 1500;
        for(int i = 0; i < numBalls; ++i) {
            MPI_Send(&data, 1, MPI_INT, 4, 0, MCW);
        }
        while(numReturned < numBalls){
            MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            ++numReturned;
        }
        for(int i = 1; i < 8; ++i){
            data = -1;
            MPI_Send(&data, 1, MPI_INT, i,0, MCW);
        }
    }
    else{
        while(data != -1) {
            MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            if(data == -1){
                break;
            }
            if(data == 6) {
                ++finished;
                MPI_Send(&rank, 1, MPI_INT, 0, 0, MCW);
            }
            else {
                passTo(rank, data);
            }
        }
    }

    std::cout << "Row " << rank << " has " << finished << " balls." << std::endl;


    MPI_Finalize();

    return 0;
}

