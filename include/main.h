
#ifndef PATHFINDING_MAIN_H
#define PATHFINDING_MAIN_H
#include "values.h"





bool endFlag = false, startFlag = false;
Tile GRID_TILES[ROWS][ROWS];
Vector2 startPos, endPos;

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
            newTile.parent = 0;
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


#endif //PATHFINDING_MAIN_H
