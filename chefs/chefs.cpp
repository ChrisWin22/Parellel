#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

#define MCW MPI_COMM_WORLD

int main(int argc, char **argv) {

    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    MPI_Status myStatus;
    MPI_Request myRequest;
    srand(rank);


    if(rank == 0) { // Cook

        while(true){
            int buf;
            int flag;
            int numOrders = 0;
            while(true) {
                MPI_Iprobe(MPI_ANY_SOURCE, 1, MCW, &flag, &myStatus);
                if(flag) {
                    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, MPI_STATUS_IGNORE);
                    ++numOrders;
                }
                else {
                    break;
                }
            }
            if(numOrders > 20) {
                std::cout << "That's too many! I quit!" << std::endl;
                for(int i = 0; i < size; ++i) {
                    MPI_Isend(&buf, 1, MPI_INT, i, 0, MCW, &myRequest);
                }
                break;
            }
            if(numOrders == 0) {
                std::cout << "Cook: Sweet, no orders. Break Time" << std::endl;
                sleep(1);
            }
            else {
                for(int i = numOrders; i > 0; --i) {
                    sleep(1);
                    std::cout << "Cook: Finished an order. " << i << " remaining." << std::endl;
                }
                std::cout << "Cook: Done with all orders, checking for more." << std::endl;
            }
            
        }
    }
    else { // Chefs
        int flag;

        while(true) {
            MPI_Iprobe(0, 0, MCW, &flag, &myStatus);

            if(flag) { // Got yelled at
                std::cout << rank << ": That was rude, I quit!" << std::endl;
                break;
            }
            else { // Give order
                int order = 1;
                int timeToWait = (rand() % 5) + 1;
                sleep(timeToWait);
                MPI_Isend(&order, 1, MPI_INT, 0, 1, MCW, &myRequest);
                std::cout << rank << ": Do this" << std::endl;
            }
        
        }
    }








    MPI_Finalize();

}
