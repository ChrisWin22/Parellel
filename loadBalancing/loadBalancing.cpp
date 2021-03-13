#include <queue>
#include <iostream>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

#define MCW MPI_COMM_WORLD

int main(int argc, char **argv) {
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    srand(rank + time(NULL));
    
//    check to see if any new work has arrived
//    if the number of tasks in the queue exceeds the threshold of 16, then 2 tasks are sent to random destinations
//    perform some work
//    for half the processes, if the number of tasks generated is less than a random number [1024-2048], generate 1-3 new tasks, and place them at the end of the process task queue.
//    for the other half of the processes, generate no new tasks.


    int receivedFlag = 0;
    int tempWork;
    std::queue<int> work;
    int numTasksGenerated = 0;
    int receivedStatusFlag = 0;
    int status = 0; // -1 = kill, 0 = no status, 1 = white token, 2 = black token
    bool passedBack = false;
    int randomNumber = (rand() % 1024) + 1024;
    //int randomNumber = 25;
    int workNumber = 0;
    bool waitingToReceive = false;

    if(rank == 0) {
        work.push(1);
        work.push(2);
    }

    while (status != -1) {
        MPI_Iprobe(MPI_ANY_SOURCE, 0, MCW, &receivedFlag, MPI_STATUS_IGNORE); // Check for work
        MPI_Iprobe(MPI_ANY_SOURCE, 1, MCW, &receivedStatusFlag, MPI_STATUS_IGNORE); // Check for status
        if(receivedStatusFlag == 1) {
            MPI_Recv(&status, 1, MPI_INT, MPI_ANY_SOURCE, 1, MCW, MPI_STATUS_IGNORE);
            std::cout << rank << ": received status " << status << std::endl;
        }

        if(rank == 0) {
            if(status == 1) {
                break;
            }
            else if(status == 2) {
                status = 0;
                waitingToReceive = false;
            }
        }

        while(receivedFlag == 1) {
            MPI_Recv(&tempWork, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            work.push(tempWork);
            MPI_Iprobe(MPI_ANY_SOURCE, 0, MCW, &receivedFlag, MPI_STATUS_IGNORE); // Check for work
        }
        if(work.size() > 16) {
            int localOne = rand() % size;
            int localTwo = rand() % size;
            int tempSendOne = work.front();
            work.pop();

            int tempSendTwo = work.front();
            work.pop();

            std::cout << rank << ": sending work to " << localOne << std::endl;
            std::cout << rank << ": sending work to " << localTwo << std::endl;

            MPI_Send(&tempSendTwo, 1, MPI_INT, localOne, 0, MCW);
            MPI_Send(&tempSendOne, 1, MPI_INT, localTwo, 0, MCW);

            if(localOne < rank || localTwo < rank) {
                passedBack = true;
            }
        }


        // Do work
        std::cout << rank << ": " << work.size() << std::endl;
        workNumber++;
        if(work.size() != 0)
            work.pop();
        //sleep(2);

        if(rank == 0 && work.size() == 0 && !waitingToReceive) {
            std::cout << rank << ": Sending white Token" << std::endl;
            int whiteBuffer = 1;
            MPI_Send(&whiteBuffer, 1, MPI_INT, (rank + 1) % size, 1, MCW);
            waitingToReceive = true;
        }

        // Do Token Stuff
        if(work.size() == 0 && status != 0 && status != -1) {
            if(passedBack || status == 2) {
                std::cout << rank << ": Sending black token" << std::endl;
                int blackBuffer = 2;
                MPI_Send(&blackBuffer, 1, MPI_INT, (rank + 1) % size, 1, MCW);
            }
            else if(status == 1) {
                std::cout << rank << ": Sending white Token" << std::endl;
                int whiteBuffer = 1;
                MPI_Send(&whiteBuffer, 1, MPI_INT, (rank + 1) % size, 1, MCW);
            }
            passedBack = false;
            status = 0;
        }
        


        if(rank % 2 == 0) {
            std::cout << rank << ": " << numTasksGenerated << "/" << randomNumber << std::endl;
            if(numTasksGenerated < randomNumber) {
                int numTasksToCreate = (rand() % 3) + 1;
                std::cout << rank << ": generate " << numTasksToCreate << std::endl;

                for(int i = 0; i < numTasksToCreate; ++i) {
                    work.push((rand() % 1024) + 1);
                    numTasksGenerated++;
                }
            }
        }

    }
    int killBuffer = -1;
    MPI_Send(&killBuffer, 1, MPI_INT, (rank + 1) % size, 1, MCW);

    MPI_Barrier(MCW);
    sleep(2);
    MPI_Barrier(MCW);

    std::cout << rank << ": Finished" << std::endl;
    
    MPI_Finalize();
    return 0;
}

