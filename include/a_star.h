//
// Created by nishu on 24-11-2023.
//

#ifndef PATHFINDING_A_STAR_H
#define PATHFINDING_A_STAR_H
#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#include "main.h"

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


#endif //PATHFINDING_A_STAR_H
