/*

Forest fire simulation with triangular tiling

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <random>
#include <chrono>
#include <vector>

#define EMPTY 0
#define TREE 1
#define FIRE 2
#define NEW_EMPTY 3 // empty for the next round
#define NEW_TREE 4 // tree for the next round
#define NEW_FIRE 5 // fire for the next round
#define P 100 // new tree probability 1/p
#define F 1000 // new fire probability 1/f
#define FIRE_PERSISTANCE 0
#define SIDE_NEIGHBORS 3 // when side touches
#define ALL_NEIGHBORS 12 // when tip touches


// to draw a square, COLUMNS*2 rows are needed
#define ROWS 150
#define COLUMNS 200
#define CELL_SIZE 4
#define FPS 30

int ** grid = nullptr;
int neighborsAmount = 3;

std::vector<std::vector<std::vector<int>>> neighbors;
std::vector<std::vector<std::vector<int>>> sideNeighbors = {
                                         {{0, -1}, {-1, 0}, {1, 0}}, // when row%2 == col%2
                                         {{-1, 0}, {1, 0}, {0, 1}}}; // when row%2 != col%2
std::vector<std::vector<std::vector<int>>> allNeighbors = {
{{2, 0}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-2, 0}, {-2, -1}, {-1, -1}, {0, -1}, {1, -1}, {2, -1}},
 {{2, 1}, {1, 1}, {0, 1}, {-1, 1}, {-2, 1}, {-2, 0}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {2, 0}}};

float sin60(std::sin(60.0 * 3.14159 / 180.0));

float colors[3][3] = {{1.0f, 1.0f, 1.0f}, // white
                      {0.0f, 1.0f, 0.0f}, // green
                      {1.0f, 0.0f, 0.0f}};  // red



void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void init_grid(int initialState);
void init_neighbors(int neigh);

void init(int initialState, int neigh) {
    glClearColor(colors[EMPTY][0], colors[EMPTY][1], colors[EMPTY][2], 0.0f);
    std::srand(std::time(0));
    init_grid(initialState);
    init_neighbors(neigh);
}

void draw_triangle(int row, int col, bool right) {
    float x(col);
    float y(row);
    y /= 2;
    if (right) {
        glBegin(GL_TRIANGLES);
          glVertex2f(x, y + 0.5);  // up
          glVertex2f(x + 1, y);    // right
          glVertex2f(x, y - 0.5);  // down
        glEnd();
    }
    else {
        glBegin(GL_TRIANGLES);
          glVertex2f(x, y);           // left
          glVertex2f(x + 1, y + 0.5); // up
          glVertex2f(x + 1, y - 0.5); // down
        glEnd();
    }
}

void draw_grid() {
    int temp;
    float ratio;
    for (int r=0; r<ROWS; r++) {
        //glColor3f(0.0, 0.0, 1.0);
        for (int c=0; c<COLUMNS; c++) {
            temp = grid[r][c];
            if (temp == FIRE) {
                glColor3f(1.0, 0.0, 0.0);
            }
            if (temp == TREE)
                glColor3f(colors[TREE][0], colors[TREE][1], colors[TREE][2]);
            if (temp == EMPTY)
                continue;
            draw_triangle(r, c, (r%2 != 0) ? (c%2) : !(c%2));
        }
    }
}
void init_grid(int initialState) {
    grid = new int * [ROWS];
    for (int i=0; i<ROWS; i++) {
        grid[i] = new int [COLUMNS];
        for (int j=0; j<COLUMNS; j++) {
            grid[i][j] = initialState;
        }
    }
}

void init_neighbors(int type) {
    neighborsAmount = type;
    if (type == SIDE_NEIGHBORS)
        neighbors = sideNeighbors;
    else
        neighbors = allNeighbors;
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
        newR = row + neighbors[row%2 != col%2][n][0];
        if (newR < 0 || newR >= ROWS) // check if out of grid
            continue;
        newC = col + neighbors[row%2 != col%2][n][1];
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
            if (grid[r][c] < 6) // to avoid touching old fires with FIRE_PERSISTANCE
                grid[r][c] %= 3;
        }
    }
}

void stats() {
    int fireCount(0), treeCount(0);
    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLUMNS; c++) {
            if (grid[r][c] == TREE)
                treeCount++;
            if (grid[r][c] == FIRE)
                fireCount++;
        }
    }
    std::cout << treeCount << "\t" << fireCount << std::endl;
}

void init() {
    glClearColor(colors[EMPTY][0], colors[EMPTY][1], colors[EMPTY][2], 0.0f);
    std::srand(std::time(0));
    init_grid(ALL_NEIGHBORS);
}

void display_callback() {
    glClear (GL_COLOR_BUFFER_BIT);

    draw_grid();

    glFlush();
    glutSwapBuffers();
}

void reshape_callback(int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, COLUMNS-0.5, ROWS/2-0.5, 0.0+0.5, -1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void timer_callback(int) {
    auto start(std::chrono::steady_clock::now());

    next_step();
    //stats();
    glutPostRedisplay(); // run the display_callback function

    auto stop(std::chrono::steady_clock::now());
    auto duration(std::chrono::duration_cast<std::chrono::milliseconds>(stop-start));
    glutTimerFunc(std::abs(1000.0/FPS - duration.count()), timer_callback, 0);
}


int main(int argc, char **argv) {
    glutInit(&argc, argv); // initialize
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(15, 15); // optional
    glutInitWindowSize(COLUMNS*CELL_SIZE, ROWS*CELL_SIZE*sin60);
    glutCreateWindow("Forest fire simulation with triangular tiling");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init(EMPTY, ALL_NEIGHBORS);
    glutMainLoop();

    return 0;
}
