/*

Forest fire simulation with hexagonal tiling

*/

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <cstdio>
#include <chrono>
#include <cmath>
#include <random>
#include <ctime>
#include <vector>

#define EMPTY 0
#define TREE 1
#define FIRE 2
#define NEW_EMPTY 3 // empty for the next round
#define NEW_TREE 4 // tree for the next round
#define NEW_FIRE 5 // fire for the next round
#define P 100 // new tree probability 1/p
#define F 1000 // fire probability 1/f

#define ROWS 60
#define COLUMNS 80
#define CELL_SIZE 10
#define FPS 10

int ** grid = nullptr;

std::vector<std::vector<int>> neighbors1 = {{-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0},  {1, 1}};
std::vector<std::vector<int>> neighbors2 = {{-1, -1}, {-1, 0}, {0, -1}, {0, 1}, {1, -1},  {1, 0}};

float cos30(std::cos(30.0 * 3.14159 / 180.0));
float sin30(std::sin(30.0 * 3.14159 / 180.0));

float hexaWidth(1.0);
float hexaSide(hexaWidth / (2 * cos30));
// to avoid superfluous calculations in draw_hexagonv
float hexaSideX(hexaSide * cos30);
float hexaSideY(hexaSide * sin30);
float hexaSideY2(hexaSide * (sin30 + 1));

// red, green, blue
float colors[3][3] = {{1.0f, 1.0f, 1.0f}, // white
                      {0.0f, 1.0f, 0.0f}, // green
                      {1.0f, 0.0f, 0.0f}  // red
                      };

void timer_callback(int);
void display_callback();
void reshape_callback(int width, int height);
void draw_hexagonv();
void draw_grid();

void draw_hexagonv(int row, int col) {
    float x(col+0.5);
    float y(row+0.25);
    float xCorr((row%2) ? 0.0 : 0.5);
    float yCorr(row * (1.0-cos30) + 0.5);
    x -= xCorr;
    y -= yCorr;
    glBegin(GL_POLYGON);
      glVertex2f(x, y);
      glVertex2f(x + hexaSideX, y + hexaSideY);
      glVertex2f(x + hexaSideX, y + hexaSideY2);
      glVertex2f(x, y + hexaSide * (2*sin30 + 1));
      glVertex2f(x - hexaSideX, y + hexaSideY2);
      //glColor3f(0.0, 0.0, 0.0);
      glVertex2f(x - hexaSideX, y + hexaSideY);
    glEnd();
}

void draw_grid() {
    int temp;
    for (int r=0; r<ROWS; r++) {
        for (int c=0; c<COLUMNS; c++) {
            temp = grid[r][c];
            if (temp == EMPTY)
                continue;
            else if (temp==FIRE)
                glColor3f(1.0, 0.0, 0.0);
            else
                glColor3f(colors[temp][0], colors[temp][1], colors[temp][2]);

            draw_hexagonv(r, c);
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

void new_tree(int row, int col) {
    if (rand()%P == 0) {
        grid[row][col] = NEW_TREE;
    }
}

void new_fire(int row, int col) {
    if (rand()%F == 0) {
        grid[row][col] = NEW_FIRE;
        return;
    }
}

void fire_around(int row, int col) {
    int newR, newC;
    int temp;
    std::vector<std::vector<int>> N;
    if (row%2)
        N = neighbors1;
    else
        N = neighbors2;
    for (std::vector<int> v : N) {
        newR = row + v[0];
        if (newR < 0 || newR >= ROWS) // check if out of grid
            continue;
        newC = col + v[1];
        if (newC < 0 || newC >= COLUMNS) // check if out of grid
            continue;
        temp = grid[newR][newC];
        // if fire or old fire or (older fire but not just created)
        if (temp == TREE) {
            grid[newR][newC] = NEW_FIRE;
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
            else if (grid[r][c] == FIRE) {
                grid[r][c] = NEW_EMPTY;
                fire_around(r, c);
            }
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
    init_grid();
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
    glOrtho(0.0, COLUMNS-0.5, ROWS*cos30-0.5, 0.0+0.5, -1, 0);
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
    glutInitWindowSize(COLUMNS*CELL_SIZE, ROWS*CELL_SIZE);
    glutCreateWindow("Forest fire simulation with hexagonal tiling");
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutTimerFunc(1000/FPS, timer_callback, 0);
    init();
    glutMainLoop();

    return 0;
}
