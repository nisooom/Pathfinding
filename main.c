#include <stdio.h>
#include "raylib.h"
#include "stdbool.h"
#include "math.h"
#include <stdlib.h>

//-----------------------------------------------
//  CHANGEABLE VARIABLES
//-----------------------------------------------

#define WIDTH 800
#define ROWS 25


//-----------------------------------------------
//  FIXED VARIABLES
//-----------------------------------------------

#define GAP (WIDTH/ROWS)
#define ASTAR_IMPLEMENTATION
#define LINKED_LIST_IMPLEMENTATION

//-----------------------------------------------
//  STRUCTS AND ENUMS
//-----------------------------------------------

enum STATE {
    CLOSED,
    OPEN,
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

//-----------------------------------------------
//  GLOBAL VARIABLES
//-----------------------------------------------

bool endFlag = false, startFlag = false;
Tile tiles[ROWS][ROWS];
Color baseTileColor;
Vector2 startPos, endPos;

Tile NULL_TILE = {0};

//-----------------------------------------------
//  Helper Functions
//-----------------------------------------------

Vector2 getRC(Tile tile) {
    Vector2 rc;
    rc.x = tile.row;
    rc.y = tile.col;
    return rc;

}

Tile* getTile(Vector2 pos) {
    return &tiles[(int) pos.x][(int) pos.y];
}

Vector2 getPos() {
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
    Vector2 pos = getPos();
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
            curr.state = TILE;
            curr.color = baseTileColor;
            break;
        case START:
            startPos.x = row, startPos.y = col;
            startFlag = !startFlag;
            curr.state = START;
            curr.color = DARKGRAY;
            break;
        case DESTINATION:
            endPos.x = row, endPos.y = col;
            endFlag = !endFlag;
            curr.state = DESTINATION;
            curr.color = BLUE;
            break;

    }
    tiles[row][col] = curr;
}

//NOTE: NOT USED YET---------------------------------------
//  PRIORITY QUEUE BY LINKED LIST IMPLEMENTATION
//---------------------------------------------------------

#ifdef LINKED_LIST_IMPLEMENTATION


struct NODE {
    Tile* tile;
    int data, priority, value;
    struct NODE *next;
} typedef NODE;

NODE *head = 0, *newNode;

void display() {
    if (head != 0) {
        NODE *current = head;
        printf("List: ");
        while (current != 0) {
            printf("%d ", current->priority);
            current = current->next;
        }
        printf("\n");
    }
}

void createNode(Tile* tile) {
    newNode = (NODE *) malloc(sizeof(NODE));
    newNode->tile = tile;
    newNode->next = 0;
    newNode->data = tile->g;
    newNode->value = tile->h;
    newNode->priority = tile->f;
}

void addNode(Tile* tile) {
    createNode(tile);
    NODE *current = head;

    if (head == NULL || current->priority < (head)->priority) {
        newNode->next = head;
        head = newNode;
    } else {
        while (current->next != NULL && current->next->priority < current->priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

Tile* pop() {

    NODE *rmv = head;
    head = head->next;
    Tile *rmvTile = rmv->tile;
    free(rmv);
    return rmvTile;

//    NODE *current = head, *prev = 0;
//    while (current->next != 0) {
//        prev = current;
//        current = current->next;
//    }
//    printf("\nRemoved Element: %d\n", current->data);
//    prev->next = 0;
//
//    return *current;

}

void clearList(){
    head = NULL;
}

#endif

//---------------------------------------------------------
//  A* IMPLEMENTATION
//---------------------------------------------------------

#ifdef ASTAR_IMPLEMENTATION


// Helper function to calculate the heuristic cost (e.g., Manhattan distance)
int heuristic(Tile *start, Tile *goal) {
    return abs(start->row - goal->row) + abs(start->col - goal->col);
}

void findNeighbours(Tile *tile) {
    int tC = tile->col;
    int tR = tile->row;

    tile->neighbours = malloc(4 * sizeof(Tile));
    tile->neighbours[0] = tiles[tR - 1][tC];
    tile->neighbours[1] = tiles[tR + 1][tC];
    tile->neighbours[2] = tiles[tR][tC - 1];
    tile->neighbours[3] = tiles[tR][tC + 1];

    for (int i = 0; i < 4; i++){
        tile->neighbours[i].parent = tile;
    }

}

// A* algorithm
void astar(Tile *start, Tile *goal) {

    Tile* currentTile = start;

    while (true) {

        if (currentTile == goal) break;

// Define offsets for the four cardinal directions (up, down, left, right)
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

// Initialize variables to keep track of the lowest f value and the corresponding neighbor
        Tile *lowestFNeighbor = NULL;
        int lowestFValue = INT_MAX;

        for (int i = 0; i < 4; i++) {
            int newRow = currentTile->row + dr[i];
            int newCol = currentTile->col + dc[i];

            // Check if the new coordinates are within the grid bounds
            if (newRow >= 0 && newRow < currentTile->totalRows &&
                newCol >= 0 && newCol < ROWS) {
                Tile *neighbor = &tiles[newRow][newCol];

                // Check if the neighbor is not in the closed set (already visited)
                if (neighbor->state != CLOSED) {
                    // Calculate the tentative g value for the neighbor
                    int tentativeG = currentTile->g + 1;  // Assuming a constant cost of 1 for movement

                    // Check if the neighbor is not in the open set or the new g value is lower
                    if (neighbor->state != OPEN || tentativeG < neighbor->g) {
                        // Set the parent of the neighbor to the current tile
                        neighbor->parent = currentTile;
                        neighbor->g = tentativeG;
                        neighbor->h = heuristic(neighbor, goal);
                        neighbor->f = neighbor->g + neighbor->h;

                        // Add the neighbor to the open set (if it's not already there)
                        if (neighbor->state != OPEN) {
                            neighbor->state = OPEN;
                        }

                        // Check if the neighbor has the lowest f value
                        if (neighbor->f < lowestFValue) {
                            lowestFValue = neighbor->f;
                            lowestFNeighbor = neighbor;
                        }
                    }
                }
            }
        }

// Mark the neighbor with the lowest f value as PATH
        if (lowestFNeighbor != NULL) {
            lowestFNeighbor->color = RED;
        }

        drawGrid();
    }


//    for(int i =0; i < ROWS; ++i)
//        for (int j = 0; j < ROWS; ++j)
//            findNeighbours(&tiles[i][j]);
//
//    Tile closedSet[ROWS*ROWS];
//    int closedIdx = 0;
//
//
//    NODE* minNeighbour = 0;
//
//
//    for (int i = 0; i < 4; ++i) {
//
//    }

//    Tile* curr = start;
//
//    while (true){
//        clearList();
//        findNeighbours(curr);
//
//        if (curr == goal) return;
//
//        for (int i = 0; i < 4; ++i) {
//
////            if (curr->neighbours[i].parent == curr) continue;
//
//            int hn = heuristic(curr, goal);
//            int gn = 0;
//            if (curr != start) gn = heuristic(start, curr);
//
//            int fn = gn + hn;
//            curr->neighbours[i].f = fn;
//            curr->neighbours[i].h = hn;
//            curr->neighbours[i].g = gn;
//
//            addNode(&curr->neighbours[i]);
//        }
//
//        Tile* next = pop();
//        next->state = PATH;
//        next->color = RED;
//        curr = next;
//      }


}

#endif
//---------------------------------------------------------
//  MAIN APP IMPLEMENTATION
//---------------------------------------------------------



void algorithm() {

    Tile *end;
    Tile *start;
    start = &tiles[(int) startPos.x][(int) startPos.y];

    end = &tiles[(int) endPos.x][(int) endPos.y];

    astar(start, end);

}

int main(void) {
    InitWindow(WIDTH, WIDTH, "Pathfinding Algorithms");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    baseTileColor = GetColor(232020);
    createGrid();
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Begin Variables


        BeginDrawing();
        drawTiles();

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_B)) changeState(BARRIER);
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




