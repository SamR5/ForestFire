/*

Forest fire simulation

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

#define EMPTY 0
#define TREE 1
#define FIRE 2
#define NEW_EMPTY 3 // empty for the next round
#define NEW_TREE 4 // tree for the next round
#define NEW_FIRE 5 // fire for the next round
#define P 100 // new tree probability 1/p
#define F 1000 // fire probability 1/f
#define FIRE_PERSISTANCE 0
#define MOORE 8
#define VON_NEUMANN 4

#define ROWS 300
#define COLUMNS 400
#define CELL_SIZE 2
#define FPS 30

int ** grid = nullptr;
int neighborsAmount;
std::vector<std::vector<int>> neighbors;

std::vector<std::vector<int>> VNNeighbors = {        {-1, 0},
                                             {0, -1},       {0, 1},
                                                     {1, 0}};

std::vector<std::vector<int>> MNeighbors = {{-1, -1}, {-1, 0}, {-1, 1},
                                            {0, -1},            {0, 1},
                                            {1, -1},   {1, 0},  {1, 1}};
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
    return {newX-1, newY-1};
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

void draw_grid() {
    int temp;
    std::vector<float> tmpCoord;
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
    //glColor3f(colors[FIRE][0], 0.0f, 0.0f);
    float redu(0.0);
    if (FIRE_PERSISTANCE == 0)
        glColor3f(1.0f, 0.0f, 0.0f);
    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLUMNS; c++) {
            if (grid[r][c] >= FIRE) {
                tmpCoord = orth_coordinates(r, c);
                if (FIRE_PERSISTANCE != 0) {
                    redu = ((float)grid[r][c] - FIRE)/FIRE_PERSISTANCE;
                    glColor3f(1.0f, 1.0f-redu, 1.0f-redu);
                }
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
    // can randomly become a fire
    if (rand()%F == 0) {
        grid[row][col] = NEW_FIRE + FIRE_PERSISTANCE;
        return;
    }
    // or put on fire if one is around
    int newR, newC;
    int temp;
    for (int n=0; n<neighborsAmount; n++) {
        newR = row + neighbors[n][0];
        if (newR < 0 || newR >= ROWS) // check if out of grid
            continue;
        newC = col + neighbors[n][1];
        if (newC < 0 || newC >= COLUMNS) // check if out of grid
            continue;
        temp = grid[newR][newC];
        // if fire or old fire or (older fire but not just created)
        if (temp == FIRE || temp == NEW_EMPTY ||
            (temp > NEW_FIRE && temp != NEW_FIRE + FIRE_PERSISTANCE)) {
            grid[row][col] = NEW_FIRE + FIRE_PERSISTANCE;
            return;
        }
    }
}

void next_step() {
    //write();
    // temporary states
    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLUMNS; c++) {
            if (grid[r][c] == EMPTY)
                new_tree(r, c);
            else if (grid[r][c] == TREE)
                new_fire(r, c);
            else if (grid[r][c] == FIRE)
                grid[r][c] = NEW_EMPTY;
            else if (grid[r][c] > NEW_FIRE)
                grid[r][c]--;
        }
    }
    // temporary states to definitive state
    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLUMNS; c++) {
            if (grid[r][c] < 6)
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

    glFlush();
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

int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(COLUMNS*CELL_SIZE, ROWS*CELL_SIZE);
    glutCreateWindow("Forest Fire Simulation");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init();
    glutMainLoop();
    return 0;
}
