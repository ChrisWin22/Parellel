#include <iostream>
#include <fstream>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

#define MCW MPI_COMM_WORLD

struct Complex {
    double real;
    double imagine;
};

Complex operator+ (Complex left, Complex right) {
    Complex answer;
    answer.real = left.real + right.real;
    answer.imagine = left.imagine + right.imagine;
    return answer;
}

Complex operator* (Complex left, Complex right) {
    Complex answer;
    answer.real = (left.real * right.real) - (left.imagine * right.imagine);
    answer.imagine = (left.real * right.imagine) + (left.imagine * right.real);
    return answer;
}

int rcolor(int num) {
    if(num == 255)
        return 0;
    return 32 * (num % 8);
}

int gcolor(int num) {
    if(num == 255)
        return 0;
    return 32 * (num % 8);
}

int bcolor(int num) {
    if(num == 255)
        return 0;
    return 32 * (num % 8);
}

int mbrot(Complex com, int maxIterations) {
    int i = 0;
    Complex temp;
    temp = com;
    while(i < maxIterations && ((temp.real * temp.real) + (temp.imagine * temp.imagine) < 4)) {
        temp = (temp * temp) + com;
        ++i;
    }
    return i;
}

struct Color {
    int r;
    int g;
    int b;
};

void printToFile(std::ofstream& fout, Color colors[], int size) {
    for(int i = 0; i < size; ++i) {
        fout << colors[i].r << " ";
        fout << colors[i].g << " ";
        fout << colors[i].b << " ";
    }
}


int main(int argc, char **argv) {

    int rank;
    int size;
    int kill;
    double data[2];
    int colors[3];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    if(rank == 0) {
        MPI_Status status;
        int temp[3];
        Color tempColor;
        std::ofstream fout;
        fout.open("mandlebrot.ppm");
        
        Complex c1, c2,c;
        c1.real = -1;
        c1.imagine = -1;
        c2.real = 1;
        c2.imagine = 1;

        int dimensions = 512;
        fout << "P3" << std::endl;
        fout << dimensions << " " << dimensions << std::endl;
        fout << "255" << std::endl;

        Color toPrint[size - 1];

        int toSendTo = 1;
        for(int j = 0; j < dimensions; ++j) {
            for(int i = 0; i < dimensions; ++i) {
                data[0] = (i * (c1.real - c2.real)/dimensions) + c2.real;
                data[1] = (j * (c1.imagine - c2.imagine)/dimensions) + c2.imagine;

                MPI_Send(&data, 2, MPI_DOUBLE, toSendTo, 0, MCW);
                toSendTo++;
                if(toSendTo == size) {
                    toSendTo = 1;
                    for(int received = 1; received < size; received++) {
                        MPI_Recv(&temp, 3, MPI_INT, MPI_ANY_SOURCE, 0, MCW, &status);
                        tempColor.r = temp[0];
                        tempColor.g = temp[1];
                        tempColor.b = temp[2];
                        toPrint[status.MPI_SOURCE - 1] = tempColor;
                    }
                    printToFile(fout, toPrint, size - 1);
                }

            }
            fout << std::endl;
        }

        for(int i = 0; i < toSendTo; ++i) {
            fout << toPrint[i].r << " " << toPrint[i].g << " " << toPrint[i].b << " ";
        }

        double kill[] = {-1, -1};

        for(int i = 1; i < size; ++i){
            MPI_Send(&kill, 2, MPI_DOUBLE, i, 0, MCW);
        }

        fout.close();

    }
    else {
        while(true) {
            MPI_Recv(&data, 2, MPI_DOUBLE, 0, 0, MCW, MPI_STATUS_IGNORE);
            if(data[0] == -1 && data[1] == -1)
                break;
            Complex c;
            c.real = data[0];
            c.imagine = data[1];
            int iterations = mbrot(c, 255);
            colors[0] = rcolor(iterations);
            colors[1] = gcolor(iterations);
            colors[2] = bcolor(iterations);
            MPI_Send(&colors, 3, MPI_INT, 0, 0, MCW);
        }

    }


    MPI_Finalize();

    return 0;

}
