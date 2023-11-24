//-----------------------------------------------
// INCLUDES
//-----------------------------------------------
#define RAYGUI_IMPLEMENTATION
#include "include/main.h"
#include "include/a_star.h"
#include "include/raygui.h"

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