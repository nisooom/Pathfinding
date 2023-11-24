//-----------------------------------------------
// INCLUDES
//-----------------------------------------------
#define RAYGUI_IMPLEMENTATION
#include <stdio.h>
#include "raylib.h"
#include "stdbool.h"
#include "math.h"
#include <stdlib.h>
#include "raygui.h"

//-----------------------------------------------
//  CHANGEABLE VARIABLES
//-----------------------------------------------

#define WIDTH 720
#define WIN_HEIGHT WIDTH
#define WIN_WIDTH (WIDTH + WIDTH * 0.3)
#define ROWS (int)(WIDTH/20)
#define FPS 60


// Base Colors

Color baseTileColor = DARKBLUE;
Color startColor = WHITE;
Color goalColor = PINK;
Color endingColor = WHITE;
Color gridLineColor = BLACK;
Color barrierColor = BLACK;
Color pathColor = RED;
Color visitedColor = ORANGE;

//-----------------------------------------------
//  FIXED VARIABLES
//-----------------------------------------------

#define GAP (int)(WIDTH/(int)ROWS)
#define A_STAR_IMPLEMENTATION
bool ALLOW_GRIDS = true;
bool ALLOW_DIAGONALS = true;

//-----------------------------------------------
//  STRUCTS AND ENUMS
//-----------------------------------------------

enum STATE {
    BARRIER,
    START,
    DESTINATION,
    UNVISITED,
    PATH
};

typedef struct Tile {
    int row, col;
    int x, y, width;
    Color color;
    enum STATE state;
    struct Tile *parent;
    int g, h, f;
} Tile;


//-----------------------------------------------
//  GLOBAL VARIABLES
//-----------------------------------------------

bool endFlag = false, startFlag = false;
Tile GRID_TILES[ROWS][ROWS];
Vector2 startPos, endPos;

//-----------------------------------------------
//  Helper Functions
//-----------------------------------------------

#define MIN(x, y) (x) > (y) ? (y): (x)

Tile *getTile(Vector2 pos) {
    return &GRID_TILES[(int) pos.x][(int) pos.y];
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
            newTile.x = newTile.row * newTile.width;
            newTile.y = newTile.col * newTile.width;
            newTile.color = baseTileColor;
            newTile.state = UNVISITED;
            newTile.parent = NULL;
            newTile.f = 0;
            newTile.g = 0;
            newTile.h = 0;

            GRID_TILES[i][j] = newTile;
        }
    }
}

void drawGrid() {

    for (int i = 0; i < ROWS; ++i) {
        DrawLineV((Vector2) {0, (float) i * (float) GAP}, (Vector2) {(float) WIDTH, (float) i * (float) GAP},
                  gridLineColor);
        for (int j = 0; j < ROWS; ++j) {
            DrawLineV((Vector2) {(float) j * (float) GAP, (float) 0},
                      (Vector2) {(float) j * (float) GAP, (float) WIDTH}, gridLineColor);
        }
    }
}

void drawTiles() {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            Tile currTile = GRID_TILES[j][i];

            DrawRectangle(currTile.x, currTile.y, currTile.width, currTile.width, currTile.color);

        }
    }

    if (ALLOW_GRIDS) drawGrid();

}

void changeState(enum STATE state) {
    Vector2 pos = getMousePos();
    int row = (int) pos.x;
    int col = (int) pos.y;

    Tile curr = GRID_TILES[row][col];

    switch (state) {

        case BARRIER:
            if (curr.state == DESTINATION) endFlag = !endFlag;
            if (curr.state == START) startFlag = !startFlag;
            curr.state = BARRIER;
            curr.color = barrierColor;
            break;
        case UNVISITED:
            if (curr.state == DESTINATION) endFlag = !endFlag;
            if (curr.state == START) startFlag = !startFlag;
            curr.state = UNVISITED;
            curr.color = baseTileColor;
            break;
        case START:
            if (curr.state == DESTINATION) endFlag = !endFlag;
            startPos.x = (float) row, startPos.y = (float) col;
            startFlag = !startFlag;
            curr.state = START;
            curr.color = startColor;
            break;
        case DESTINATION:
            if (curr.state == START) startFlag = !startFlag;
            endPos.x = (float) row, endPos.y = (float) col;
            endFlag = !endFlag;
            curr.state = DESTINATION;
            curr.color = goalColor;
            break;

    }
    GRID_TILES[row][col] = curr;
}

void updateTileColor(Tile *tile, Color col) {

    GRID_TILES[tile->row][tile->col].color = col;

    BeginDrawing();
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            Tile currTile = GRID_TILES[j][i];
            Tile *start_tmp = getTile(startPos);
            Tile *end_tmp = getTile(startPos);
            if (start_tmp == &currTile) {
                DrawRectangle(currTile.x, currTile.y, currTile.width, currTile.width, startColor);
            } else if (end_tmp == &currTile) {
                DrawRectangle(currTile.x, currTile.y, currTile.width, currTile.width, endingColor);
            } else {
                DrawRectangle(currTile.x, currTile.y, currTile.width, currTile.width, currTile.color);
            }
        }
    }
    if (ALLOW_GRIDS) drawGrid();
    EndDrawing();

}

void clearGrid(){
    for(int i = 0; i < ROWS; i ++){
        for (int j = 0; j < ROWS; j++){
            Tile* curTile = &GRID_TILES[i][j];
            if (curTile->state == PATH){
                curTile->color = baseTileColor;
                curTile->state = UNVISITED;
            }
        }
    }

    Tile* start = getTile(startPos);
    Tile* end = getTile(endPos);

    start->color = startColor;
    end->color = endingColor;
    start->state = START;
    end->state = DESTINATION;
    endFlag = !endFlag;
    startFlag = !startFlag;


}


//---------------------------------------------------------
//  A* IMPLEMENTATION
//---------------------------------------------------------

#ifdef A_STAR_IMPLEMENTATION

typedef struct {
    int x, y;
    int g, h, f;
    bool obstacle;
    struct Node *parent;
} Node;

Node *createNode(int x, int y, bool obstacle) {
    Node *node = (Node *) malloc(sizeof(Node));
    node->x = x;
    node->y = y;
    node->g = 0;
    node->h = 0;
    node->f = 0;
    node->obstacle = obstacle;
    node->parent = NULL;
    return node;
}

int heuristic(Node *a, Node *b) {

    int dx = abs(a->x - b->x);
    int dy = abs(a->y - b->y);
    if (ALLOW_DIAGONALS) {
        return (int) ((dx + dy) + (1.414 - 2) * MIN(dx, dy));
    } else {
        return dx + dy;
    }
}

bool isValid(int x, int y) {
    return x >= 0 && x < ROWS && y >= 0 && y < ROWS;
}

bool isTraversable(Node *grid[ROWS][ROWS], int x, int y) {
    return isValid(x, y) && !grid[x][y]->obstacle;
}

Node *findMinF(Node *openSet[ROWS][ROWS]) {
    Node *minNode = NULL;
    int minF = INT_MAX;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < ROWS; j++) {
            Node *current = openSet[i][j];
            if (current != NULL && current->f < minF) {
                minF = current->f;
                minNode = current;
            }
        }
    }
    return minNode;
}

void aStar(Node *start, Node *goal) {

    Node *grid[ROWS][ROWS];

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < ROWS; ++j) {
            Tile curr = GRID_TILES[i][j];
            grid[i][j] = createNode(curr.row, curr.col, (curr.state == BARRIER));
        }
    }

    Node *openSet[ROWS][ROWS] = {NULL};
    Node *closedSet[ROWS][ROWS] = {NULL};

    openSet[start->x][start->y] = start;

    while (true) {


        Node *current = findMinF(openSet);

        if (current == NULL) {
            printf("No path found.\n");
            return;
        }

        if (current->x == goal->x && current->y == goal->y) {
            // Path found, trace and print it
            printf("Path found: ");
            while (current != NULL) {

                Tile *curTile = getTile((Vector2) {(float) current->x, (float) current->y});

                updateTileColor(curTile, pathColor);
                if (current != start || current != goal) curTile->state = PATH;

                printf("(%d, %d) ", current->x, current->y);
                current = (Node *) current->parent;
            }
            printf("\n");
            return;
        }

        openSet[current->x][current->y] = NULL;
        closedSet[current->x][current->y] = current;

        // Generate neighbors


        int dx[] = {1, -1, 0, 0, 1, 1, -1, -1};
        int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};

        for (int i = 0; i < ((ALLOW_DIAGONALS) ? 8 : 4); i++) {
            int newX = current->x + dx[i];
            int newY = current->y + dy[i];

            if (isTraversable(grid, newX, newY)) {
                Node *neighbor = grid[newX][newY];

                if (closedSet[newX][newY] == NULL) {
                    int tentativeG = current->g + 1;

                    if (openSet[newX][newY] == NULL || tentativeG < neighbor->g) {
                        neighbor->g = tentativeG;
                        neighbor->h = heuristic(neighbor, goal);
                        neighbor->f = neighbor->g + neighbor->h;
                        neighbor->parent = (struct Node *) current;

                        Tile *curTile = getTile((Vector2) {(float) current->x, (float) current->y});

                        updateTileColor(curTile, visitedColor);
                        if (current != start || current != goal) curTile->state = PATH;

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
//  MAIN APP IMPLEMENTATION & GUI
//---------------------------------------------------------

void algorithm() {

    aStar(createNode((int) startPos.x, (int) startPos.y, false), createNode((int) endPos.x, (int) endPos.y, false));

    updateTileColor(getTile(startPos), endingColor);
    updateTileColor(getTile(endPos), endingColor);

    startFlag = !startFlag;
    endFlag = !endFlag;
}

void createButtons() {


    (ALLOW_GRIDS) ? GuiSetState(STATE_FOCUSED) : GuiSetState(STATE_NORMAL);
    if (GuiButton((Rectangle){ WIN_WIDTH * 0.8, WIN_HEIGHT * 0.1, 120, 40 }, "Toggle Grid")) {
        ALLOW_GRIDS = !ALLOW_GRIDS;
    }

    (ALLOW_DIAGONALS) ? GuiSetState(STATE_FOCUSED) : GuiSetState(STATE_NORMAL);
    if (GuiButton((Rectangle){ WIN_WIDTH * 0.8, WIN_HEIGHT * 0.2, 160, 40 }, "Toggle Diagonals")) {
        ALLOW_DIAGONALS = !ALLOW_DIAGONALS;
    }

    GuiSetState(STATE_NORMAL);
    if (GuiButton((Rectangle){ WIN_WIDTH * 0.8, WIN_HEIGHT * 0.3, 100, 40 }, "Run Algorithm")) {
        algorithm();
    }
    GuiSetState(STATE_NORMAL);
    if (GuiButton((Rectangle){ WIN_WIDTH * 0.8, WIN_HEIGHT * 0.4, 100, 40 }, "Reset")) {
        createGrid();
    }
    GuiSetState(STATE_NORMAL);
    if (GuiButton((Rectangle){ WIN_WIDTH * 0.8, WIN_HEIGHT * 0.5, 100, 40 }, "Clear")) {
        clearGrid();
    }

    int fontSize = 20;

    DrawText("For Walls:", WIN_WIDTH * 0.8, WIN_HEIGHT * 0.6, fontSize, BLACK);
    DrawText("B or Right Click", WIN_WIDTH * 0.8, WIN_HEIGHT * 0.64, fontSize, BLACK);

    DrawText("For Start:", WIN_WIDTH * 0.8, WIN_HEIGHT * 0.7, fontSize, BLACK);
    DrawText("S", WIN_WIDTH * 0.8, WIN_HEIGHT * 0.74, fontSize, BLACK);

    DrawText("For Goal:", WIN_WIDTH * 0.8, WIN_HEIGHT * 0.8, fontSize, BLACK);
    DrawText("F", WIN_WIDTH * 0.8, WIN_HEIGHT * 0.84, fontSize, BLACK);

}

void checkEvents() {

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsKeyDown(KEY_B)) changeState(BARRIER);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) changeState(UNVISITED);

    if ((!startFlag) && (IsKeyPressed(KEY_S))) changeState(START);
    if ((!endFlag) && (IsKeyPressed(KEY_F))) changeState(DESTINATION);

    if (IsKeyPressed(KEY_R)) createGrid();
    if (IsKeyPressed(KEY_SPACE)) algorithm();

    if (IsKeyPressed(KEY_C)) clearGrid();

    if (IsKeyPressed(KEY_G)) ALLOW_GRIDS = !ALLOW_GRIDS;


}


void mainEventLoop() {

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "A* Pathfinding Algorithm");
    SetTargetFPS(FPS);

    createGrid();

    while (!WindowShouldClose()) {
        BeginDrawing();

        drawTiles();
        createButtons();
        checkEvents();

        ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
}


int main(void) {

    mainEventLoop();

    return 0;
}