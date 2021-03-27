#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define MCW MPI_COMM_WORLD

// Reset bases after inning
// limit to 9 innings

enum Result {
    MISS,
    SINGLE,
    DOUBLE,
    OUT
};

enum Tag {
    PITCH,
    HIT,
    RESULT,
    GAMEOVER
};

enum Position{
    PITCHER,
    BATTER,
    FIELDER
};

enum Pitch {
    FASTBALL,
    CURVEBALL,
    CHANGEUP 
};

enum Bases {
    FIRST,
    SECOND,
    THIRD,
    HOME
};

std::string pitchToString(Pitch p) {
    switch (p) {
        case Pitch::FASTBALL: return "Fastball";
        case Pitch::CHANGEUP: return "Changeup";
        case Pitch::CURVEBALL: return "Curveball";
        default: return "AHHH IDK!!!!";
    }
}

int getPitchDifficulty(Pitch pitch) {
    switch (pitch) {
        case Pitch::FASTBALL: return 1;
        case Pitch::CHANGEUP: return 2;
        case Pitch::CURVEBALL: return 3;
        default: return 1;
    }
}

std::string rankToPosition(int rank) {
    switch (rank) {
        case 2: return "1B";
        case 3: return "2B";
        case 4: return "SS";
        case 5: return "3B";
        case 6: return "LF";
        case 7: return "CF";
        case 8: return "RF";
        default: return "Coach";
    }
}

std::string resultToString(int result) {
    switch (result) {
        case Result::DOUBLE: return "DOUBLE";
        case Result::SINGLE: return "SINGLE";
        default: return "doesn't matter";
    }
}

void runPitcher() {
    int data;
    MPI_Status status;
    int awayScore = 0;
    int homeScore = 0;
    int inning = 1;
    bool topOfInning = true;
    int strikes = 0;
    int outs = 0;
    bool running = true;
    std::array<int, 3> runners;

    for(int i = 0; i < runners.size(); ++i) {
        runners[i] = 0;
    }

    while(running) {
        // Throw pitch
        int pitch = rand() % 3;
        std::cout << "PITCHER: pitched a " << pitchToString(Pitch(pitch)) << std::endl;
        MPI_Send(&pitch, 1, MPI_INT, 1, Tag::PITCH, MCW);

        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, Tag::RESULT, MCW, &status);

        if(data != Result::MISS) {
            std::cout << "Ball hit to " << rankToPosition(status.MPI_SOURCE) << std::endl;
        }

        if(data == Result::OUT) {
            std::cout << "OUT!!" << std::endl;
            if(++outs == 3) {
                std::cout << "End of inning" << std::endl;
                for(int i = 0; i < runners.size(); ++i) {
                    runners[i] = 0;
                }
                outs = 0;
                if(!topOfInning) {
                    topOfInning = true;
                    if(++inning == 10) {
                        running = false;
                    }
                }
                else {
                    topOfInning = false;
                }
            }
        }
        else if(data == Result::MISS) {
            std::cout << "BATTER: missed, strike " << strikes << std::endl;
            if(++strikes == 3) {
                strikes = 0;
                std::cout << "Batter struck out" << std::endl;
                if(++outs == 3) {
                    std::cout << "End of inning" << std::endl;
                    for(int i = 0; i < runners.size(); ++i) {
                        runners[i] = 0;
                    }
                    outs = 0;
                    if(!topOfInning) {
                        topOfInning = true;
                        if(++inning == 10) {
                            running = false;
                        }
                    }
                    else {
                        topOfInning = false;
                    }
                }
            }
        }
        else if(data == Result::SINGLE || data == Result::DOUBLE) {
            std::cout << resultToString(data) << std::endl;
            bool addedHitter = false;
            for(int i = 0; i < runners.size(); ++i) {
                if(runners[i] != 0) {
                    runners[i] += data;
                    if(runners[i] > 3) {
                        std::cout << "SCORE!!!" << std::endl;
                        runners[i] = 0;
                        if(topOfInning) {
                            awayScore++;
                        }
                        else{
                            homeScore++;
                        }
                    }
                }
                else if(!addedHitter) {
                    addedHitter = true;
                    runners[i] += data;
                }
            }
            
        }
    }
    std::cout << "Game Over!\nFinal Score:\nAway: " << awayScore << "\nHome: " << homeScore << std::endl;
}

void runBatter() {
    int data;
    MPI_Status status;

    while(true) {
        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &status);

        if(status.MPI_TAG == Tag::GAMEOVER) {
            break;
        }

        if(status.MPI_TAG == Tag::PITCH) {
            Pitch pitch = Pitch(data);
            float swing = (rand() % 100) + 1;
            float attempt = swing / getPitchDifficulty(pitch); 
            if(attempt > 25) { // Hit
                int hitTo = (rand() % 7) + 2;
                int hitDifficutly = rand() % 100;
                MPI_Send(&hitDifficutly, 1, MPI_INT, hitTo, Tag::HIT, MCW);
            }
            else {
                int miss = Result::MISS;
                MPI_Send(&miss, 1, MPI_INT, Position::PITCHER, Tag::RESULT, MCW);
            }

        }
    }


}

void runFielder() {
    int data;
    MPI_Status status;
    while(true) {
        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MCW, &status);

        if(status.MPI_TAG == Tag::GAMEOVER) {
            break;
        }
        
        int attempt = rand() % 100;

        int diff = attempt - data;
        int message;

        if(diff > 0) {
            message = Result::OUT;
        }
        else if(diff > -30) {
            message = Result::SINGLE;
        }
        else {
            message = Result::DOUBLE;
        }
        MPI_Send(&message, 1, MPI_INT, Position::PITCHER, Tag::RESULT, MCW);
    }

}

void runGame(Position pos) {

    if(pos == PITCHER) {
        runPitcher();
        int pill = 0xDEADBEEF;
        for(int i = 1; i < 9; ++i) {
            MPI_Send(&pill, 1, MPI_INT, i, Tag::GAMEOVER, MCW);
        }
    }
    else if(pos == BATTER) {
        runBatter();
    }
    else {
        runFielder();
    }

}
Position setPosition(int rank) {
    switch(rank) {
        case 0: return PITCHER;
        case 1: return BATTER;
        default: return FIELDER;
    }
}

int main(int argc, char **argv) {

    int rank, size;
    Position position;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    srand(rank + time(NULL));

    position = setPosition(rank);

    runGame(position);

    MPI_Finalize();
    return 0;
}
