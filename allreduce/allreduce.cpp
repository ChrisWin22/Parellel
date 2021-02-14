#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>

#define MCW MPI_COMM_WORLD

void sendReceive(int rank, int myNumber, int size) {
    int total;
    if(rank == 0) {
        std::cout << "=============================" << std::endl;
        std::cout << "Starting Send and Receive... " << std::endl;
        std::cout << "=============================" << std::endl;
        
        total = myNumber;
        int temp;
        for(int i = 1; i < size; ++i) {
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            total += temp;
        }
    
        for(int i = 1; i < size; ++i) {
            MPI_Send(&total, 1, MPI_INT, i, 0, MCW);
        }
        sleep(1);
    }
    else {
        MPI_Send(&myNumber, 1, MPI_INT, 0, 0, MCW);
        MPI_Recv(&total, 1, MPI_INT, 0, 0, MCW, MPI_STATUS_IGNORE);
        std::cout << rank << ": Received total is " << total << std::endl;
    }
}

void bcast(int rank, int myNumber, int size) {
    int total = 0;
    int totalArray[size];

    if (rank == 0) {
        totalArray[0] = myNumber;
        std::cout << "=============================" << std::endl;
        std::cout << "Starting Gather and BCast..." << std::endl;
        std::cout << "=============================" << std::endl;
    }

    MPI_Gather(&myNumber, 1, MPI_INT, totalArray, 1, MPI_INT, 0, MCW);

    if(rank == 0) {
        for(int i : totalArray) {
            total += i;
        }
    }

    MPI_Bcast(&total, 1, MPI_INT, 0, MCW);

    if(rank != 0) {
        std::cout << rank << ": Received total is " << total << std::endl;
    }
    else {
        sleep(1);
    }

}

void allReduce(int rank, int myNumber) {
    int total = 0;

    if (rank == 0) {
        std::cout << "=============================" << std::endl;
        std::cout << "Starting AllReduce..." << std::endl;
        std::cout << "=============================" << std::endl;
    }
    else {
        sleep(1);
    }
    MPI_Allreduce(&myNumber, &total, 1, MPI_INT, MPI_SUM, MCW);

    std::cout << rank << ": Received total is " << total << std::endl;

}

void ring(int rank, int myNumber, int size) {
    int total = 0;
    int received;
    if(rank == 0) {
        std::cout << "=============================" << std::endl;
        std::cout << "Starting Ring..." << std::endl;
        std::cout << "=============================" << std::endl;

        MPI_Send(&myNumber, 1, MPI_INT, (rank + 1)%size, 0, MCW);
        MPI_Recv(&total, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
        for(int i = 1; i < size; ++i) {
            MPI_Send(&total, 1, MPI_INT, i, 0, MCW);
        }
    }
    else {
        sleep(1);
        MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
        total = received + myNumber;
        MPI_Send(&total, 1, MPI_INT, (rank + 1)%size, 0, MCW);
        MPI_Recv(&total, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
    }

    std::cout << rank << ": Recieved total is " << total << std::endl;

}

void cube(int rank, int myNumber) {

}

int main(int argc, char **argv) {

    int rank;
    int size;
    int myNumber;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    srand(rank + time(NULL));

    myNumber = (rand()%10) + 1;

    sendReceive(rank, myNumber, size);
    bcast(rank, myNumber, size);
    allReduce(rank, myNumber);
    ring(rank, myNumber, size);

    

    MPI_Finalize();
}



