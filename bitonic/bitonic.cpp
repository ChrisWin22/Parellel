#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define MCW MPI_COMM_WORLD

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
    }
    MPI_Barrier(MCW);

    std::cout << rank << ": My starting number is " << myNumber << std::endl;
    MPI_Barrier(MCW);
    if(rank == 0) {
        std::cout << "==================" << std::endl;
        std::cout << "After Sort Numbers" << std::endl;
        std::cout << "==================" << std::endl;
    }
    MPI_Barrier(MCW);

    for(int loop = 0; loop < timesToLoop; ++loop) {
       int destination = rank^(mask<<loop); 
       std::cout << rank << ": Trading with " << destination << " on loop " << loop << std::endl;
       MPI_Send(&myNumber, 1, MPI_INT, destination, 0, MCW);
       MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);

       if(destination > rank) {
           myNumber = std::min(temp, myNumber);
       }
       else {
           myNumber = std::max(temp, myNumber);
       }
       MPI_Barrier(MCW);
    }

    std::cout << rank << ": My ending number is " << myNumber << std::endl;


	MPI_Finalize();

	return 0;
}
