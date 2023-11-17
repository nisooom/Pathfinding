#include <stdio.h>
#include "raylib.h"
#include "stdbool.h"
#include "math.h"
#include <stdlib.h>

//-----------------------------------------------
//  CHANGEABLE VARIABLES
//-----------------------------------------------

#define WIDTH 720
#define ROWS 30
#define FPS 60

//-----------------------------------------------
//  FIXED VARIABLES
//-----------------------------------------------

#define GAP (WIDTH/ROWS)
#define ASTAR_IMPLEMENTATION

//-----------------------------------------------
//  STRUCTS AND ENUMS
//-----------------------------------------------

enum STATE {
    BARRIER,
    START,
    DESTINATION,
    UNVISITED
};

typedef struct Tile {
    int row, col;
    int x, y, width;
    Color color;
    struct Tile *neighbours;
    int totalRows;
    enum STATE state;
    struct Tile *parent;
    int g, h, f;
} Tile;

typedef struct {
    int x, y;
    int g, h, f;
    bool obstacle;
    struct Node* parent;
} Node;

//-----------------------------------------------
//  GLOBAL VARIABLES
//-----------------------------------------------

bool endFlag = false, startFlag = false;
Tile tiles[ROWS][ROWS];
Color baseTileColor = DARKBLUE;
Vector2 startPos, endPos;

//-----------------------------------------------
//  Helper Functions
//-----------------------------------------------

Tile* getTile(Vector2 pos) {
    return &tiles[(int) pos.x][(int) pos.y];
}

Vector2 getMousePos() {
    Vector2 pos;
    Vector2 mousePos = GetMousePosition();
    pos.x = mousePos.x / GAP;
    pos.y = mousePos.y / GAP;
    return pos;
}

void createGrid() {

    startFlag = false;
    endFlag = false;

    int gap = WIDTH / ROWS;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < ROWS; j++) {
            Tile newTile;
            newTile.row = i;
            newTile.col = j;
            newTile.width = gap;
            newTile.totalRows = ROWS;
            newTile.x = newTile.row * newTile.width;
            newTile.y = newTile.col * newTile.width;
            newTile.color = baseTileColor;
            newTile.state = UNVISITED;
            newTile.totalRows = ROWS;
            newTile.parent = NULL;
            newTile.f = 0;
            newTile.g = 0;
            newTile.h = 0;

            tiles[i][j] = newTile;
//            openSet[i][j] = newTile;
        }
    }
}

void drawGrid() {
    int gap = WIDTH / ROWS;
    for (int i = 0; i < ROWS; ++i) {
        DrawLineV((Vector2) {0, i * gap}, (Vector2) {WIDTH, i * gap}, BLACK);
        for (int j = 0; j < ROWS; ++j) {
            DrawLineV((Vector2) {j * gap, 0}, (Vector2) {j * gap, WIDTH}, BLACK);
        }
    }
}

void drawTiles() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            Tile currTile = tiles[j][i];

            DrawRectangle(currTile.x, currTile.y, currTile.width, currTile.width, currTile.color);

        }
    }
    drawGrid();
}

void changeState(enum STATE state) {
    Vector2 pos = getMousePos();
    int row = pos.x;
    int col = pos.y;

    Tile curr = tiles[row][col];

    switch (state) {

        case BARRIER:
            if (curr.state == DESTINATION) endFlag = !endFlag;
            if (curr.state == START) startFlag = !startFlag;
            curr.state = BARRIER;
            curr.color = BLACK;
            break;
        case UNVISITED:
            if (curr.state == DESTINATION) endFlag = !endFlag;
            if (curr.state == START) startFlag = !startFlag;
            curr.state = UNVISITED;
            curr.color = baseTileColor;
            break;
        case START:
            startPos.x = row, startPos.y = col;
            startFlag = !startFlag;
            curr.state = START;
            curr.color = WHITE;
            break;
        case DESTINATION:
            endPos.x = row, endPos.y = col;
            endFlag = !endFlag;
            curr.state = DESTINATION;
            curr.color = PURPLE;
            break;

    }
    tiles[row][col] = curr;
}

void updateTileColor(Tile* tile, Color col){

    tiles[tile->row][tile->col].color = col;

    BeginDrawing();
    WaitTime(0.01);
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            Tile currTile = tiles[j][i];

            DrawRectangle(currTile.x, currTile.y, currTile.width, currTile.width, currTile.color);
        }
    }
    drawGrid();
    EndDrawing();

//    drawTiles();

}


//---------------------------------------------------------
//  A* IMPLEMENTATION
//---------------------------------------------------------

#ifdef ASTAR_IMPLEMENTATION

Node* createNode(int x, int y, bool obstacle) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->x = x;
    node->y = y;
    node->g = 0;
    node->h = 0;
    node->f = 0;
    node->obstacle = obstacle;
    node->parent = NULL;
    return node;
}

int heuristic(Node* a, Node* b) {
    return abs(a->x - b->x) + abs(a->y - b->y);
}

bool isValid(int x, int y) {
    return x >= 0 && x < ROWS && y >= 0 && y < ROWS;
}

bool isTraversable(Node* grid[ROWS][ROWS], int x, int y) {
    return isValid(x, y) && !grid[x][y]->obstacle;
}

Node* findMinF(Node* openSet[ROWS][ROWS]) {
    Node* minNode = NULL;
    int minF = INT_MAX;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < ROWS; j++) {
            Node* current = openSet[i][j];
            if (current != NULL && current->f < minF) {
                minF = current->f;
                minNode = current;
            }
        }
    }
    return minNode;
}

void astar(Node* start, Node* goal) {

    Node* grid[ROWS][ROWS];

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            Tile curr = tiles[i][j];
            grid[i][j] = createNode(curr.row, curr.col, (curr.state == BARRIER));
        }
    }

    Node* openSet[ROWS][ROWS] = {NULL};
    Node* closedSet[ROWS][ROWS] = {NULL};

    openSet[start->x][start->y] = start;

    while (true) {

        WaitTime(0.1);

        Node* current = findMinF(openSet);

        if (current == NULL) {
            printf("No path found.\n");
            return;
        }

        if (current->x == goal->x && current->y == goal->y) {
            // Path found, trace and print it
            printf("Path found: ");
            while (current != NULL) {

                Tile* curTile = getTile((Vector2){current->x, current->y});


                updateTileColor(curTile, RED);
                printf("(%d, %d) ", current->x, current->y);
                current = (Node *) current->parent;
            }
            printf("\n");
            return;
        }

        openSet[current->x][current->y] = NULL;
        closedSet[current->x][current->y] = current;

        // Generate neighbors
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};

        for (int i = 0; i < 4; i++) {
            int newX = current->x + dx[i];
            int newY = current->y + dy[i];

            if (isTraversable(grid, newX, newY)) {
                Node* neighbor = grid[newX][newY];

                if (closedSet[newX][newY] == NULL) {
                    int tentativeG = current->g + 1;

                    if (openSet[newX][newY] == NULL || tentativeG < neighbor->g) {
                        neighbor->g = tentativeG;
                        neighbor->h = heuristic(neighbor, goal);
                        neighbor->f = neighbor->g + neighbor->h;
                        neighbor->parent = (struct Node *) current;

                        Tile* curTile = getTile((Vector2){current->x, current->y});

                        updateTileColor(curTile, YELLOW);


                        if (openSet[newX][newY] == NULL) {
                            openSet[newX][newY] = neighbor;
                        }
                    }
                }
            }
        }
    }
}

#endif


//---------------------------------------------------------
//  MAIN APP IMPLEMENTATION
//---------------------------------------------------------

void algorithm() {

    astar(createNode(startPos.x, startPos.y, false), createNode(endPos.x, endPos.y, false));

}

int main(void) {

    InitWindow(WIDTH, WIDTH, "A* Pathfinding Algorithm");
    SetTargetFPS(FPS);

    createGrid();
    while (!WindowShouldClose()){

        BeginDrawing();
        drawTiles();

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsKeyDown(KEY_B)) changeState(BARRIER);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) changeState(UNVISITED);

        if ((!startFlag) && (IsKeyPressed(KEY_S))) changeState(START);
        if ((!endFlag) && (IsKeyPressed(KEY_F))) changeState(DESTINATION);

        if (IsKeyPressed(KEY_R)) createGrid();
        if (IsKeyPressed(KEY_SPACE)) algorithm();

        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}