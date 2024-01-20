//
// Created by nishu on 04-12-2023.
//

#ifndef PATHFINDING_VALUES_H
#define PATHFINDING_VALUES_H
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"

#define WIDTH 720
#define WIN_HEIGHT WIDTH
#define WIN_WIDTH (WIDTH + WIDTH * 0.3)
#define ROWS (int)(WIDTH/20)
#define FPS 60



Color baseTileColor = DARKBLUE;
Color startColor = WHITE;
Color goalColor = PINK;
Color endingColor = WHITE;
Color gridLineColor = BLACK;
Color barrierColor = BLACK;
Color pathColor = RED;
Color visitedColor = ORANGE;


#define GAP (int)(WIDTH/(int)ROWS)
#define A_STAR_IMPLEMENTATION
bool ALLOW_GRIDS = true;
bool ALLOW_DIAGONALS = true;
bool error = false;

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

#endif //PATHFINDING_VALUES_H
