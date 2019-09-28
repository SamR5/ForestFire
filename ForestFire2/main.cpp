/*

Forest fire simulation with only a line of fire at the bottom

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <ctime>
#include <random>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <cmath>


// the bottom line is permanently on fire
#define EMPTY 0
#define TREE 1
#define FIRE 2
#define NEW_EMPTY 3 // empty for the next round
#define NEW_TREE 4 // tree for the next round
#define NEW_FIRE 5 // fire for the next round
#define P 100 // new tree probability 1/p
#define MOORE 8
#define VON_NEUMANN 4

#define ROWS 300
#define COLUMNS 400
#define CELL_SIZE 2
#define FPS 30


void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void draw_grid();

int ** grid = nullptr;
int neighborsAmount;
std::vector<std::vector<int>> neighbors;

std::vector<std::vector<int>> VNNeighbors = {        {-1, 0},
                                             {0, -1},       {0, 1},
                                                     {1, 0}
                                            };

std::vector<std::vector<int>> MNeighbors = {{-1, -1}, {-1, 0}, {-1, 1},
                                            {0, -1},            {0, 1},
                                            {1, -1},   {1, 0},  {1, 1}
                                           };
// red, green, blue
float colors[3][3] = {{1.0f, 1.0f, 1.0f}, // white
                      {0.0f, 1.0f, 0.0f}, // green
                      {1.0f, 0.0f, 0.0f}  // red
                      };
// -1 < X < 1 and -1 < Y < 1
float scaleR(2.0f / ROWS);
float scaleC(2.0f / COLUMNS);


std::vector<float> orth_coordinates(int row, int col) {
    std::vector<float> res;
    float newX(col);
    float newY(ROWS - row);
    newX *= 2.0 / COLUMNS;
    newY *= 2.0 / ROWS;
    //return {newX-1, newY-1};
    res.push_back(newX-1);
    res.push_back(newY-1);
    return res;
}

void write() {
    std::ofstream myFile("temp.txt");
    if (myFile) {
        for (int r=0; r<ROWS; r++) {
            for (int c=0; c<COLUMNS; c++) {
                myFile << grid[r][c] << " ";
            }
            myFile << std::endl;
        }
    }
}

void write_time(int duration) {
    std::ofstream myFile("tmp2.txt", std::ios::app);
    if (myFile) {
        myFile << duration << std::endl;
    }
}

void draw_grid() {
    int temp;
    std::vector<float> tmpCoord;
    // this decomposition avoids changing color at each cell (faster)
    glColor3f(colors[TREE][0], colors[TREE][1], colors[TREE][2]);
    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLUMNS; c++) {
            if (grid[r][c] == TREE) { // the background is the same color as empty cells
                tmpCoord = orth_coordinates(r, c);
                glRectf(tmpCoord[0], tmpCoord[1],
                        tmpCoord[0] + scaleC, tmpCoord[1] + scaleR);
            }
        }
    }
    glColor3f(colors[FIRE][0], colors[FIRE][1], colors[FIRE][2]);
    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLUMNS; c++) {
            if (grid[r][c] == FIRE) { // the background is the same color as empty cells
                tmpCoord = orth_coordinates(r, c);
                glRectf(tmpCoord[0], tmpCoord[1],
                        tmpCoord[0] + scaleC, tmpCoord[1] + scaleR);
            }
        }
    }
}

void init_grid() {
    grid = new int * [ROWS];
    for (int i=0; i<ROWS; i++) {
        grid[i] = new int [COLUMNS];
        for (int j=0; j<COLUMNS; j++) {
            //grid[i][j] = (rand()%100 < treeOdds) ? TREE : EMPTY;
            if (i == ROWS-1)
                grid[i][j] = FIRE;
            else
                grid[i][j] = EMPTY;
        }
    }
}

void init_neighbors(int type) {
    neighborsAmount = type;
    switch (type) {
        case VON_NEUMANN:
            neighbors = VNNeighbors; break;
        case MOORE:
            neighbors = MNeighbors; break;
    }
}

void new_tree(int row, int col) {
    if (rand()%P == 0) {
        grid[row][col] = NEW_TREE;
    }
}

void new_fire(int row, int col) {
    int newR, newC;
    for (int n=0; n<neighborsAmount; n++) {
        newR = row + neighbors[n][0];
        if (newR < 0 || newR >= ROWS) // check if out of grid
            continue;
        newC = col + neighbors[n][1];
        if (newC < 0 || newC >= COLUMNS) // check if out of grid
            continue;
        if (grid[newR][newC] == FIRE || grid[newR][newC] == NEW_EMPTY) {
            grid[row][col] = NEW_FIRE;
            return;
        }
    }
}

void next_step() {
    // temporary states
    for (int r=0; r<ROWS-1; r++) {
        for (int c=0; c<COLUMNS; c++) {
            switch (grid[r][c]) {
                case EMPTY:
                    new_tree(r, c); break;
                case TREE:
                    new_fire(r, c); break;
                case FIRE:
                    grid[r][c] = NEW_EMPTY; break;
            }
        }
    }
    // temporary states to definitive state
    for (int r=0; r<ROWS-1; r++) {
        for (int c=0; c<COLUMNS; c++) {
            grid[r][c] %= 3;
        }
    }
}

void init() {
    glClearColor(colors[EMPTY][0], colors[EMPTY][1], colors[EMPTY][2], 0.0f);
    init_grid();
    init_neighbors(MOORE);
}

void display_callback() {
    auto start(std::chrono::steady_clock::now());
    
    
    glClear (GL_COLOR_BUFFER_BIT);
    draw_grid();

    glutSwapBuffers();

    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    next_step();
    glutPostRedisplay(); // run the display_callback function
    glutTimerFunc(1000.0/FPS, timer_callback, 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(COLUMNS*CELL_SIZE, ROWS*CELL_SIZE);
    glutCreateWindow("Forest Fire simulation");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}
