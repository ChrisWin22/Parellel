#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define MCW MPI_COMM_WORLD

void sendReceive(int rank, int myNumber, int size) {
    int total;
    if(rank == 0) {
        total = myNumber;
        int temp;
        for(int i = 1; i < size; ++i) {
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            total += temp;
        }
    
        for(int i = 1; i < size; ++i) {
            MPI_Send(&total, 1, MPI_INT, i, 0, MCW);
        }
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
}

void allReduce(int rank, int myNumber) {
    int total = 0;

    MPI_Allreduce(&myNumber, &total, 1, MPI_INT, MPI_SUM, MCW);

    std::cout << rank << ": Received total is " << total << std::endl;

}

void ring(int rank, int myNumber, int size) {
    int total = 0;
    int received;
    if(rank == 0) {
        MPI_Send(&myNumber, 1, MPI_INT, (rank + 1)%size, 0, MCW);
        MPI_Recv(&total, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
        for(int i = 1; i < size; ++i) {
            MPI_Send(&total, 1, MPI_INT, i, 0, MCW);
        }
    }
    else {
        MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
        total = received + myNumber;
        MPI_Send(&total, 1, MPI_INT, (rank + 1)%size, 0, MCW);
        MPI_Recv(&total, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
    }

    std::cout << rank << ": Recieved total is " << total << std::endl;

}

void cube(int rank, int myNumber, int size) {
    int timesToLoop = log2(size);
    int total = myNumber;
    int mask = 1;
    int temp = 0;

    for(int loop = 0; loop < timesToLoop; ++loop) {
        int destination = rank^(mask<<loop);
        MPI_Send(&total, 1, MPI_INT, destination, 0, MCW);
        MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
        total += temp;
        MPI_Barrier(MCW);
    }
    std::cout << rank << ": Recieved total is " << total << std::endl;
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

    
    MPI_Barrier(MCW);
    if(rank == 0) {
        sleep(1);
        std::cout << "=============================" << std::endl;
        std::cout << "Starting Send and Receive..." << std::endl;
        std::cout << "=============================" << std::endl;
    } 
    else {
        sleep(2);
    }
    MPI_Barrier(MCW);
    sendReceive(rank, myNumber, size);

    // Starting Gather and Bcast
    MPI_Barrier(MCW);
    if(rank == 0) {
        sleep(1);
        std::cout << "=============================" << std::endl;
        std::cout << "Starting Gather and Bcast..." << std::endl;
        std::cout << "=============================" << std::endl;
    } 
    else {
        sleep(2);
    }
    MPI_Barrier(MCW);
    bcast(rank, myNumber, size);

    // Starting All Reduce
    MPI_Barrier(MCW);
    if(rank == 0) {
        sleep(1);
        std::cout << "=============================" << std::endl;
        std::cout << "Starting All Reduce..." << std::endl;
        std::cout << "=============================" << std::endl;
    } 
    else {
        sleep(2);
    }
    MPI_Barrier(MCW);
    allReduce(rank, myNumber);

    // Starting Ring
    MPI_Barrier(MCW);
    if(rank == 0) {
        sleep(1);
        std::cout << "=============================" << std::endl;
        std::cout << "Starting Ring..." << std::endl;
        std::cout << "=============================" << std::endl;
    } 
    else {
        sleep(2);
    }
    MPI_Barrier(MCW);
    ring(rank, myNumber, size);

    // Starting Cube
    MPI_Barrier(MCW);
    if(rank == 0) {
        sleep(1);
        std::cout << "=============================" << std::endl;
        std::cout << "Starting Cube..." << std::endl;
        std::cout << "=============================" << std::endl;
    } 
    else {
        sleep(2);
    }
    MPI_Barrier(MCW);
    cube(rank, myNumber, size);

    

    MPI_Finalize();
}



