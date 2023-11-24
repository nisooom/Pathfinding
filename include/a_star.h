//
// Created by nishu on 24-11-2023.
//

#ifndef PATHFINDING_A_STAR_H
#define PATHFINDING_A_STAR_H
#include "stdbool.h"
#include "stdlib.h"
#include "raylib.h"
#include "main.h"
#include "stdio.h"

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


#endif //PATHFINDING_A_STAR_H
