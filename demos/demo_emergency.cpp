#include "raylib.h"
#include "vehicles/vehicle.h"
#include "core/position.h"
#include "core/enums.h"

int main() {
    InitWindow(1000, 700, "Smart City - Emergency Vehicle Demo");
    SetTargetFPS(60);
    
    // Créer quelques véhicules
    Vehicle ambulance(TYPE_AMBULANCE, Position(200, 300), WHITE);
    Vehicle police(TYPE_POLICE, Position(400, 300), BLUE);
    Vehicle fireTruck(TYPE_FIRE_TRUCK, Position(600, 300), RED);
    
    float timer = 0;
    
    while (!WindowShouldClose()) {
        timer += GetFrameTime();
        
        BeginDrawing();
        ClearBackground(Color{35, 75, 35, 255});
        
        DrawText("DEMONSTRATION VEHICULES D'URGENCE", 200, 50, 30, YELLOW);
        DrawText("Ambulance - Police - Pompiers", 300, 100, 24, WHITE);
        
        // Dessiner les véhicules
        ambulance.draw();
        police.draw();
        fireTruck.draw();
        
        // Informations
        DrawText("AMBULANCE", 180, 350, 20, WHITE);
        DrawText("POLICE", 380, 350, 20, WHITE);
        DrawText("POMPIERS", 580, 350, 20, WHITE);
        
        // Effets clignotants
        if (((int)(timer * 4) % 2) == 0) {
            DrawCircle(200, 280, 8, RED);
            DrawCircle(400, 280, 8, BLUE);
            DrawCircle(600, 280, 8, RED);
        } else {
            DrawCircle(200, 280, 8, BLUE);
            DrawCircle(400, 280, 8, RED);
            DrawCircle(600, 280, 8, BLUE);
        }
        
        DrawText("Pressez ESC pour quitter", 350, 650, 20, LIGHTGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}