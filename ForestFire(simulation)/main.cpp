/*

Forest fire simulation

*/

#include <iostream>
#include <ctime>
#include <random>
#include <vector>
#include <string>
#include <fstream>

#define EMPTY 0
#define TREE 1
#define FIRE 2
#define ASHES 3
#define TEMP_FIRE 4


int height;
int width;
int ** grid = nullptr;

std::vector<std::vector<std::vector<int>>> neighbors = {
{{-1, 0}, {0, -1}, {0, 1}, {1, 0}},
{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}
};
int neighborIndex;

void init_grid(int treeOdds) {
    grid = new int * [height];
    for (int i=0; i<height; i++) {
        grid[i] = new int [width];
        for (int j=0; j<width; j++) {
            grid[i][j] = (rand()%100 < treeOdds) ? TREE : EMPTY;
        }
    }
    grid[height/2][width/2] = FIRE;
}

void write_results(int density, double burnt, double steps) {
    std::string neigType = (neighborIndex==0) ? "VonNeumann_" : "Moore_";
    std::string fileName(std::to_string(height)+"x"+std::to_string(width));
    std::ofstream myFile(neigType + fileName + ".csv",
                         std::ios::app);
    if (myFile) {
        myFile << density << ";" << burnt << ";" << steps << std::endl;
    }
    else {
        std::cout << "error " << fileName << std::endl;
    }
    //std::rename("result/" + fileName + ".tmp", "result/" + fileName + ".txt");
}

bool is_fire_around(int row, int col) {
    int newR, newC;
    for (std::vector<int> n : neighbors[neighborIndex]) {
        if (grid[n[0]+row][n[1]+col] == FIRE)
            return true;
    }
    return false;
}

bool next_step() {
    bool is_any_on_fire(false); // to tell if any tree has been put on fire
    // puts the trees on temporary fire
    for (int r=1; r<height-1; r++) {
        for (int c=1; c<width-1; c++) {
            if (grid[r][c] == TREE) {
                if (is_fire_around(r, c)) {
                    is_any_on_fire = true;
                    grid[r][c] = TEMP_FIRE;
                }
            }
        }
    }
    // puts the fires in ashes and the temporary fires to fire
    for (int r=1; r<height-1; r++) {
        for (int c=1; c<width-1; c++) {
            switch (grid[r][c]) {
                case FIRE:
                    grid[r][c] = ASHES;
                    break;
                case TEMP_FIRE:
                    grid[r][c] = FIRE;
            }
        }
    }
    // will return false when all fires will become ashes
    return is_any_on_fire;
}

std::vector<int> stats() {
    // empty, tree, fire, ashes
    std::vector<int> res(4, 0);
    int trees, empty, ashes;
    for (int r=1; r<height-1; r++) {
        for (int c=1; c<width-1; c++) {
            res[grid[r][c]]++;
        }
    }
    return res;
}

void launch_simulation(int h, int w, int amountOfTests, int neigI) {
    height = h;
    width = w;
    neighborIndex = neigI;
    std::vector<int> tempStats;
    for (int to=1; to < 100; to++) {
        std::cout << to << std::endl;
        double trees(0.0); // remaining
        double ashes(0.0);
        double totalSteps(0);
        for (int n=0; n<amountOfTests; n++) {
            init_grid(to);
            int steps(0);
            while (next_step())
                steps++;
            tempStats = stats();
            trees += tempStats[TREE]; // adds the remaining trees
            ashes += tempStats[ASHES];
            totalSteps += steps;
        }
        write_results(to, ashes/(trees+ashes), totalSteps / amountOfTests);
    }
}

int main(int argc, char** argv) {
    //system("pause");
    std::srand(std::time(0));
    // height, width, amount of test, 0=Von Neumann and 1=Moore neighbors
    launch_simulation(101, 101, 100, 0); 
    launch_simulation(101, 101, 100, 1); // height, width, amount of test
    return 0;
}
