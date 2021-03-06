#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <limits.h>


#define MCW MPI_COMM_WORLD

struct city {
    int x;
    int y;
};

const std::array<city, 100> cities {{
    {179140,750703},
    {78270,737081},
    {577860,689926},
    {628150,597095},
    {954030,510314},
    {837880,811285},
    {410640, 846947},
    {287850, 600161},
    {270030, 494359},
    {559020, 199445},
    {353930, 542989},
    {515920, 497472},
    {648080, 470280},
    {594550, 968799},
    {386690, 907669},
    { 93070, 395385},
    { 93620, 313966},
    {426870,  39662},
    {437000, 139949},
    {789810, 488001},
    {749130, 575522},
    {481030, 286118},
    {670720, 392925},
    {273890, 892877},
    {138430, 562658},
    { 85480, 465869},
    {775340, 220065},
    {862980, 312238},
    {155180, 263662},
    {274070,  74689},
    {333340, 456245},
    {822150, 399803},
    {158880, 612518},
    {815560, 707417},
    {678240, 709341},
    {394470, 679221},
    {631300, 846813},
    {528320, 824193},
    {666940, 845130},
    {298650, 816352},
    {243750, 745443},
    {220500, 654221},
    {338920, 381007},
    {313110, 201386},
    {856380, 564703},
    {549250, 565255},
    {537400, 604425},
    {502110, 435463},
    {498840, 590729},
    {482310, 571034},
    {416930, 765126},
    {418400, 638700},
    {374170, 695851},
    {412370, 570904},
    {301090, 737412},
    {235690, 782470},
    {475940, 439645},
    {268540, 609753},
    {130500, 712663},
    { 81660, 732470},
    { 64520, 711936},
    {264690, 529248},
    { 90230, 612484},
    { 38370, 610277},
    { 15430, 579032},
    {138890, 482432},
    {264580, 421188},
    { 86690, 394738},
    {209190, 347661},
    {425890, 376154},
    {312480, 177450},
    {373360, 142350},
    {442850, 106198},
    {505100, 189757},
    {542610, 224170},
    {566730, 262940},
    {615970, 237922},
    {612120, 303181},
    {634410, 320152},
    {879480, 239867},
    {868760, 286928},
    {807670, 334613},
    {943060, 368070},
    {827280, 387076},
    {896040, 413699},
    {920900, 454842},
    {746380, 440559},
    {734300, 452247},
    {730780, 471211},
    {870570, 549620},
    {607060, 453077},
    {926580, 669624},
    {812660, 614479},
    {701420, 559132},
    {688600, 580646},
    {743800, 669521},
    {819700, 857004},
    {683690, 682649},
    {732680, 857362},
    {685760, 866857},
}};


double getDistanceBetweenToPoints(int cityA, int cityB) {
    city a = cities[cityA];
    city b = cities[cityB];

    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double getDistanceOfArray(std::array<int, 100> path) {
    double total = 0;
    for(int i = 0; i < 99; ++i) {
        total += getDistanceBetweenToPoints(path[i], path[i + 1]);
    }
    return total;
}

void swapNumbersInArray(int* array, int num1, int num2) {
    int localOne = -1;
    int localTwo = -1;

    for(int i = 0; i < 100; ++i) {
        if(array[i] == num1) {
            localOne = i;
        }
        else if(array[i] == num2) {
            localTwo = i;
        }
    }

    int temp = array[localOne];
    array[localOne] = array[localTwo];
    array[localTwo] = temp;

}

int getNumLocation(std::array<int,100> array, int num) {
    for(int i = 0; i < 100; ++i) {
        if(array[i] == num) {
            return i;
        }
    }
    return -1;
}

void mutate(std::array<int,100> &child) {
    int start = rand() % 100;
    int end = rand() % 100;

    if(start > end) {
        int temp = start;
        start = end;
        end = temp;
    }

    while(start < end) {
        int temp = child[start];
        child[start] = child[end];
        child[end] = temp;

        start++;
        end--;
    }
}

std::array<int,100> makeChild(std::array<int, 100> parentA, std::array<int, 100> parentB) {
    std::array<int,100> child = parentA;

    // int start = rand() % 100;
    // int end = rand() % 100;
    int start = 20;
    int end = 40;

    if(start > end) {
        int temp = start;
        start = end;
        end = temp;
    }

    for(int i = start; i < end; ++i) {
        int p1Num = parentA[i];
        int p2Num = parentB[i];

        int p1Local = getNumLocation(parentA, p1Num);
        int p2Local = getNumLocation(parentA, p2Num);

        swapNumbersInArray(&child[0], child[p1Local], child[p2Local]);
    }

    mutate(child);

    return child;
}

bool compareDistance(std::array<int, 100> left, std::array<int,100> right) {
    return (getDistanceOfArray(left) < getDistanceOfArray(right));
}

int main(int argc, char **argv) {
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    srand(rank + time(NULL));

    if(rank == 0) {
        double best = std::numeric_limits<double>::max();
        double temp;
        for(int i = 0; i < size - 1; ++i) {                
            MPI_Recv(&temp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            if(temp < best) {
                best = temp;
            }
        }
        std::cout << "The best distance is: " << best << std::endl;
    }
    else {
        std::array<std::array<int, 100>, 10> population;
        for(int i = 0; i < 10; ++i) {
            std::array<int, 100> parent;

            for(int j = 0; j < 100; ++j) {
                parent[j] = j;
            }

            for(int k = 0; k < 50; ++k) {
                int num1 = rand() % 100;
                int num2 = rand() % 100;
                swapNumbersInArray(&parent[0], num1, num2);
            }
            population[i] = parent;
        }

        for(int j = 0; j < 200000; ++j){
            std::array<std::array<int, 100>, 20> nextGen;
            for(int i = 0; i < 20; ++i) {
                int parentASpot = rand() % 10;
                int parentBSpot = rand() % 10;
                while(parentASpot == parentBSpot) {
                    parentBSpot = rand() % 10;
                }

                nextGen[i] = makeChild(population[parentASpot], population[parentBSpot]);
            }
            std::sort(std::begin(nextGen), std::end(nextGen), compareDistance);
            for(int i = 0; i < 10; ++i) {
                population[i] = nextGen[i];
            }
        }
        double bestSolution = getDistanceOfArray(population[0]);
        std::cout << rank << ": " << bestSolution << std::endl;

        MPI_Send(&bestSolution, 1, MPI_DOUBLE, 0, 0, MCW);
    }

    MPI_Finalize();
}