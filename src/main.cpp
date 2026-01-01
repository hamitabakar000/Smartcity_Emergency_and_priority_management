#include "raylib.h"
#include "raymath.h"
#include "simulation/simulation.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <queue>
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1400, 900, "Smart City - Emergency Command Center");
    SetTargetFPS(60);
    Simulation simulation;
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            simulation.handleMouseClick(GetMousePosition());
        }
        simulation.update(deltaTime);
        BeginDrawing();
        simulation.draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
