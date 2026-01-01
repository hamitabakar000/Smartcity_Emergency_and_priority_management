#include "simulation/simulation.h"
#include "core/constants.h"
#include "core/enums.h"
#include "core/position.h"
#include <algorithm>
Simulation::Simulation() : civilianSpawnTimer(0), nextEmergencyId(1),
               hospitalPos(80, 80),
               policeStationPos(WINDOW_WIDTH - 180, 80),
               fireStationPos(80, WINDOW_HEIGHT - 180) {
    initializeCity();
    createEmergencyVehicles();
}
Simulation::~Simulation() {
    cleanup();
}
void Simulation::cleanup() {
    for (auto v : vehicles) delete v;
    for (auto l : trafficLights) delete l;
    vehicles.clear();
    trafficLights.clear();
    emergencies.clear();
}
void Simulation::reset() {
    cleanup();
    civilianSpawnTimer = 0;
    nextEmergencyId = 1;
    initializeCity();
    createEmergencyVehicles();
}
void Simulation::initializeCity() {
    for (int row = 1; row < GRID_SIZE; row++) {
        for (int col = 1; col < GRID_SIZE; col++) {
            float x = col * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
            float y = row * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
            trafficLights.push_back(new TrafficLight(x, y));
        }
    }
}
void Simulation::createEmergencyVehicles() {
    vehicles.push_back(new Vehicle(TYPE_AMBULANCE, 
        Position(hospitalPos.x + 30, hospitalPos.y + 70), WHITE));
    vehicles.push_back(new Vehicle(TYPE_AMBULANCE, 
        Position(hospitalPos.x + 80, hospitalPos.y + 70), WHITE));
    vehicles.push_back(new Vehicle(TYPE_POLICE,
        Position(policeStationPos.x + 30, policeStationPos.y + 70), BLUE));
    vehicles.push_back(new Vehicle(TYPE_POLICE,
        Position(policeStationPos.x + 80, policeStationPos.y + 70), BLUE));
    vehicles.push_back(new Vehicle(TYPE_FIRE_TRUCK,
        Position(fireStationPos.x + 30, fireStationPos.y + 70), RED));
    vehicles.push_back(new Vehicle(TYPE_FIRE_TRUCK,
        Position(fireStationPos.x + 80, fireStationPos.y + 70), RED));
}
void Simulation::update(float deltaTime) {
    for (auto light : trafficLights) {
        light->update(deltaTime);
    }
    civilianSpawnTimer += deltaTime;
    if (civilianSpawnTimer > 2.0f && vehicles.size() < 60) {
        spawnCivilianVehicle();
        civilianSpawnTimer = 0;
    }
    for (int i = vehicles.size() - 1; i >= 0; i--) {
        vehicles[i]->update(deltaTime, trafficLights, vehicles, roadNetwork);
        if (!vehicles[i]->isActive() && vehicles[i]->getType() == TYPE_CIVILIAN) {
            delete vehicles[i];
            vehicles.erase(vehicles.begin() + i);
        }
    }
    for (auto& emerg : emergencies) {
        if (emerg.active) {
            emerg.timer += deltaTime;
            if (!emerg.assigned) {
                assignVehicleToEmergency(&emerg);
            }
        }
    }
    emergencies.erase(
        std::remove_if(emergencies.begin(), emergencies.end(),
            [](const Emergency& e) { return !e.active && e.timer > 7.0f; }),
        emergencies.end()
    );
}
void Simulation::spawnCivilianVehicle() {
    int edge = GetRandomValue(0, 3);
    Position startPos;
    Direction startDir;
    int roadIndex = GetRandomValue(0, GRID_SIZE);
    float roadCoord = roadIndex * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
    switch(edge) {
        case 0:
            startPos = Position(roadCoord, 30);
            startDir = DIR_SOUTH;
            break;
        case 1:
            startPos = Position(roadCoord, WINDOW_HEIGHT - 30);
            startDir = DIR_NORTH;
            break;
        case 2:
            startPos = Position(30, roadCoord);
            startDir = DIR_EAST;
            break;
        case 3:
            startPos = Position(WINDOW_WIDTH - 30, roadCoord);
            startDir = DIR_WEST;
            break;
    }
    Color colors[] = {
        Color{40, 70, 140, 255},
        Color{140, 40, 40, 255},
        Color{40, 140, 40, 255},
        Color{140, 140, 40, 255},
        Color{100, 40, 140, 255},
        Color{140, 80, 40, 255}
    };
    Color randomColor = colors[GetRandomValue(0, 5)];
    Vehicle* newVehicle = new Vehicle(TYPE_CIVILIAN, startPos, randomColor);
    vehicles.push_back(newVehicle);
}
void Simulation::createEmergency(EmergencyType type, Position pos) {
    Emergency newEmerg;
    newEmerg.type = type;
    newEmerg.position = pos;
    newEmerg.active = true;
    newEmerg.assigned = false;
    newEmerg.timer = 0;
    newEmerg.id = nextEmergencyId++;
    emergencies.push_back(newEmerg);
}
void Simulation::assignVehicleToEmergency(Emergency* emergency) {
    if (!emergency || emergency->assigned) return;
    VehicleType requiredType;
    switch(emergency->type) {
        case EMERG_FIRE: requiredType = TYPE_FIRE_TRUCK; break;
        case EMERG_MEDICAL: requiredType = TYPE_AMBULANCE; break;
        case EMERG_SECURITY: requiredType = TYPE_POLICE; break;
        case EMERG_ACCIDENT: requiredType = TYPE_POLICE; break;
        default: requiredType = TYPE_AMBULANCE; break;
    }
    Vehicle* closestVehicle = nullptr;
    float minDist = FLT_MAX;
    for (auto v : vehicles) {
        if (v->getType() == requiredType && v->getStatus() == STATUS_IDLE) {
            float dist = v->getPosition().distanceTo(emergency->position);
            if (dist < minDist) {
                minDist = dist;
                closestVehicle = v;
            }
        }
    }
    if (closestVehicle) {
        closestVehicle->assignMission(emergency, roadNetwork);
    }
}
void Simulation::draw() const {
    ClearBackground(Color{35, 75, 35, 255});
    drawRoadGrid();
    drawBuildings();
    for (auto light : trafficLights) {
        light->draw();
    }
    for (const auto& emerg : emergencies) {
        if (emerg.active) {
            drawEmergency(emerg);
        }
    }
    for (auto v : vehicles) {
        v->draw();
    }
    drawUI();
}
void Simulation::drawRoadGrid() const {
    for (int row = 0; row <= GRID_SIZE; row++) {
        float y = row * (CELL_SIZE + ROAD_WIDTH);
        DrawRectangle(0, y, WINDOW_WIDTH, ROAD_WIDTH, Color{60, 60, 60, 255});
        for (int x = 0; x < WINDOW_WIDTH; x += 40) {
            DrawRectangle(x, y + ROAD_WIDTH/2 - 2, 25, 4, YELLOW);
        }
        DrawRectangle(0, y, WINDOW_WIDTH, 2, WHITE);
        DrawRectangle(0, y + ROAD_WIDTH - 2, WINDOW_WIDTH, 2, WHITE);
    }
    for (int col = 0; col <= GRID_SIZE; col++) {
        float x = col * (CELL_SIZE + ROAD_WIDTH);
        DrawRectangle(x, 0, ROAD_WIDTH, WINDOW_HEIGHT, Color{60, 60, 60, 255});
        for (int y = 0; y < WINDOW_HEIGHT; y += 40) {
            DrawRectangle(x + ROAD_WIDTH/2 - 2, y, 4, 25, YELLOW);
        }
        DrawRectangle(x, 0, 2, WINDOW_HEIGHT, WHITE);
        DrawRectangle(x + ROAD_WIDTH - 2, 0, 2, WINDOW_HEIGHT, WHITE);
    }
}
void Simulation::drawBuildings() const {
    Color blockColors[] = {
        Color{90, 130, 90, 255},
        Color{80, 120, 80, 255},
        Color{100, 140, 100, 255},
        Color{85, 125, 85, 255},
        Color{95, 135, 95, 255}
    };
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            float x = col * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH;
            float y = row * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH;
            int colorIndex = (row * GRID_SIZE + col) % 5;
            Color blockColor = blockColors[colorIndex];
            DrawRectangle(x + 5, y + 5, CELL_SIZE - 10, CELL_SIZE - 10, blockColor);
            DrawRectangleLines(x + 5, y + 5, CELL_SIZE - 10, CELL_SIZE - 10, DARKGRAY);
            for (int wx = 0; wx < 3; wx++) {
                for (int wy = 0; wy < 3; wy++) {
                    float winX = x + 20 + wx * 30;
                    float winY = y + 20 + wy * 30;
                    DrawRectangle(winX, winY, 10, 10, YELLOW);
                }
            }
        }
    }
    DrawRectangleRec(Rectangle{hospitalPos.x - 50, hospitalPos.y - 30, 140, 120}, LIGHTGRAY);
    DrawRectangleLines(hospitalPos.x - 50, hospitalPos.y - 30, 140, 120, BLACK);
    DrawText("HOPITAL", hospitalPos.x - 25, hospitalPos.y + 60, 14, RED);
    DrawRectangle(hospitalPos.x + 5, hospitalPos.y - 10, 20, 40, RED);
    DrawRectangle(hospitalPos.x - 5, hospitalPos.y, 40, 20, RED);
    DrawRectangleRec(Rectangle{policeStationPos.x - 50, policeStationPos.y - 30, 140, 120}, 
                    Color{100, 100, 200, 255});
    DrawRectangleLines(policeStationPos.x - 50, policeStationPos.y - 30, 140, 120, BLACK);
    DrawText("POLICE", policeStationPos.x - 20, policeStationPos.y + 60, 14, WHITE);
    DrawCircle(policeStationPos.x + 15, policeStationPos.y, 22, DARKBLUE);
    DrawText("P", policeStationPos.x + 7, policeStationPos.y - 12, 28, WHITE);
    DrawRectangleRec(Rectangle{fireStationPos.x - 50, fireStationPos.y - 30, 140, 120}, 
                    Color{200, 100, 100, 255});
    DrawRectangleLines(fireStationPos.x - 50, fireStationPos.y - 30, 140, 120, BLACK);
    DrawText("POMPIERS", fireStationPos.x - 30, fireStationPos.y + 60, 14, WHITE);
    DrawText("F", fireStationPos.x + 8, fireStationPos.y - 8, 38, ORANGE);
}
void Simulation::drawEmergency(const Emergency& emerg) const {
    Color emergColor;
    const char* emergText;
    switch(emerg.type) {
        case EMERG_FIRE:
            emergColor = ORANGE;
            emergText = "INCENDIE";
            break;
        case EMERG_MEDICAL:
            emergColor = Color{0, 255, 100, 255};
            emergText = "URGENCE MEDICALE";
            break;
        case EMERG_SECURITY:
            emergColor = BLUE;
            emergText = "POLICE";
            break;
        case EMERG_ACCIDENT:
            emergColor = YELLOW;
            emergText = "ACCIDENT";
            break;
    }
    int pulse = (int)(GetTime() * 4) % 2;
    float radius = pulse ? 55 : 48;
    DrawCircleGradient(emerg.position.x, emerg.position.y, radius, 
                      Fade(emergColor, 0.9f), Fade(emergColor, 0.1f));
    DrawCircleLines(emerg.position.x, emerg.position.y, radius + 5, emergColor);
    DrawCircleLines(emerg.position.x, emerg.position.y, radius + 8, Fade(emergColor, 0.5f));
    DrawText(emergText, emerg.position.x - MeasureText(emergText, 15)/2,
            emerg.position.y - 75, 15, WHITE);
    DrawText("!", emerg.position.x - 10, emerg.position.y - 18, 35, WHITE);
}
void Simulation::drawUI() const {
    DrawRectangle(0, 0, WINDOW_WIDTH, 50, Fade(BLACK, 0.85f));
    DrawText("SMART CITY - SYSTEME DE GESTION D'URGENCE", WINDOW_WIDTH/2 - 280, 15, 22, YELLOW);
    DrawRectangle(10, 60, 160, 58, RED);
    DrawRectangleLines(10, 60, 160, 58, BLACK);
    DrawText("INCENDIE", 42, 82, 20, WHITE);
    DrawRectangle(10, 128, 160, 58, GREEN);
    DrawRectangleLines(10, 128, 160, 58, BLACK);
    DrawText("MEDICAL", 45, 150, 20, WHITE);
    DrawRectangle(10, 196, 160, 58, BLUE);
    DrawRectangleLines(10, 196, 160, 58, BLACK);
    DrawText("POLICE", 55, 218, 20, WHITE);
    DrawRectangle(10, 264, 160, 58, YELLOW);
    DrawRectangleLines(10, 264, 160, 58, BLACK);
    DrawText("ACCIDENT", 35, 286, 20, BLACK);
    DrawRectangle(10, 332, 160, 58, ORANGE);
    DrawRectangleLines(10, 332, 160, 58, BLACK);
    DrawText("RESET", 55, 354, 20, WHITE);
    DrawRectangle(10, 400, 160, 50, Fade(GRAY, 0.8f));
    DrawText("1. Cliquez type", 15, 406, 11, WHITE);
    DrawText("2. Cliquez carte", 15, 423, 11, WHITE);
    DrawText("3. Attendez 5s...", 17, 436, 11, YELLOW);
    int civilCount = 0, emergCount = 0, onMission = 0, idle = 0, returning = 0;
    for (auto v : vehicles) {
        if (v->getType() == TYPE_CIVILIAN) {
            civilCount++;
        } else {
            emergCount++;
            if (v->getStatus() == STATUS_EN_ROUTE) {
                onMission++;
            } else if (v->getStatus() == STATUS_IDLE) {
                idle++;
            } else if (v->getStatus() == STATUS_RETURNING) {
                returning++;
            }
        }
    }
    DrawRectangle(10, WINDOW_HEIGHT - 200, 220, 190, Fade(BLACK, 0.88f));
    DrawRectangleLines(10, WINDOW_HEIGHT - 200, 220, 190, YELLOW);
    DrawText("=== STATISTIQUES ===", 28, WINDOW_HEIGHT - 192, 14, YELLOW);
    DrawText(TextFormat("Total vehicules: %d", (int)vehicles.size()), 
             20, WINDOW_HEIGHT - 170, 13, WHITE);
    DrawText(TextFormat("Vehicules civils: %d", civilCount), 
             20, WINDOW_HEIGHT - 148, 13, SKYBLUE);
    DrawText(TextFormat("Vehicules urgence: %d", emergCount), 
             20, WINDOW_HEIGHT - 126, 13, WHITE);
    DrawText(TextFormat("En mission: %d", onMission), 
             20, WINDOW_HEIGHT - 104, 13, ORANGE);
    DrawText(TextFormat("En retour: %d", returning), 
             20, WINDOW_HEIGHT - 82, 13, VIOLET);
    DrawText(TextFormat("Disponibles: %d", idle), 
             20, WINDOW_HEIGHT - 60, 13, LIME);
    DrawText(TextFormat("Urgences actives: %d", (int)emergencies.size()), 
             20, WINDOW_HEIGHT - 38, 13, RED);
    DrawText(TextFormat("Feux rouges: %d", (int)trafficLights.size()), 
             20, WINDOW_HEIGHT - 16, 13, YELLOW);
    DrawText(TextFormat("FPS: %d", GetFPS()), WINDOW_WIDTH - 90, 12, 16, LIME);
}
void Simulation::handleMouseClick(Vector2 mousePos) {
    static EmergencyType selectedType = EMERG_FIRE;
    static bool typeSelected = false;
    if (CheckCollisionPointRec(mousePos, Rectangle{10, 332, 160, 58})) {
        reset();
        typeSelected = false;
        return;
    }
    if (CheckCollisionPointRec(mousePos, Rectangle{10, 60, 160, 58})) {
        selectedType = EMERG_FIRE;
        typeSelected = true;
    } else if (CheckCollisionPointRec(mousePos, Rectangle{10, 128, 160, 58})) {
        selectedType = EMERG_MEDICAL;
        typeSelected = true;
    } else if (CheckCollisionPointRec(mousePos, Rectangle{10, 196, 160, 58})) {
        selectedType = EMERG_SECURITY;
        typeSelected = true;
    } else if (CheckCollisionPointRec(mousePos, Rectangle{10, 264, 160, 58})) {
        selectedType = EMERG_ACCIDENT;
        typeSelected = true;
    } else if (typeSelected && mousePos.x > 200 && mousePos.y > 100 &&
               mousePos.x < WINDOW_WIDTH - 50 && mousePos.y < WINDOW_HEIGHT - 50) {
        createEmergency(selectedType, Position(mousePos.x, mousePos.y));
        typeSelected = false;
    }
}
