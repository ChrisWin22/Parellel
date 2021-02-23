#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define MCW MPI_COMM_WORLD

int getIthBit(int rank, int spot) {
    return rank & (1 << spot);
}

int main(int argc, char **argv) {
	int rank, size;
	int data;
    int mask = 1;
    int temp = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MCW, &rank);
	MPI_Comm_size(MCW, &size);
    srand(rank + time(NULL));

    int myNumber = (rand()%10) + 1;

    int timesToLoop = log2(size);

    MPI_Barrier(MCW);

    if(rank == 0) {
        std::cout << "================" << std::endl;
        std::cout << "Starting Numbers" << std::endl;
        std::cout << "================" << std::endl;
        sleep(1);
    }
    MPI_Barrier(MCW);

    std::cout << rank << ": My starting number is " << myNumber << std::endl;
    MPI_Barrier(MCW);
    if(rank == 0) {
        std::cout << "==================" << std::endl;
        std::cout << "After Sort Numbers" << std::endl;
        std::cout << "==================" << std::endl;
        sleep(1);
    }
    MPI_Barrier(MCW);

    for(int loop = 0; loop < timesToLoop; ++loop) {
        for(int innerLoop = loop; innerLoop >= 0; --innerLoop) {
            int destination = rank^(mask<<innerLoop); 
            MPI_Send(&myNumber, 1, MPI_INT, destination, 0, MCW);
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);

            if(getIthBit(rank, loop+1)){
                // Desc
                if(rank > destination) {
                    myNumber = std::min(temp, myNumber);
                }
                else {
                    myNumber = std::max(temp, myNumber);
                }
            }
            else {
                // Asc
                if(rank < destination) {
                    myNumber = std::min(temp, myNumber);
                }
                else {
                    myNumber = std::max(temp, myNumber);
                }
            }

            //if(destination > rank && !getIthBit(rank, loop + 1)) {
            //    myNumber = std::min(temp, myNumber);
            //}
            //else if(rank > destination && getIthBit(rank, loop + 1)) {
            //    myNumber = std::min(temp, myNumber);
            //}
            //else { 
            //    myNumber = std::max(temp, myNumber);
            //}
            
            MPI_Barrier(MCW);
        }
    }

    std::cout << rank << ": My ending number is " << myNumber << std::endl;


	MPI_Finalize();

	return 0;
}
