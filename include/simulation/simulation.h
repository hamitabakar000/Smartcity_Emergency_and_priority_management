#ifndef SIMULATION_H
#define SIMULATION_H
#include <vector>
#include "raylib.h"
#include "../vehicles/vehicle.h"
#include "../infrastructure/traffic_light.h"
#include "../infrastructure/road_network.h"
class Simulation {
private:
    std::vector<Vehicle*> vehicles;
    std::vector<TrafficLight*> trafficLights;
    std::vector<Emergency> emergencies;
    RoadNetwork roadNetwork;
    float civilianSpawnTimer;
    int nextEmergencyId;
    Position hospitalPos;
    Position policeStationPos;
    Position fireStationPos;
public:
    Simulation();
    ~Simulation();
    void cleanup();
    void reset();
    void update(float deltaTime);
    void draw() const;
    void handleMouseClick(Vector2 mousePos);
private:
    void initializeCity();
    void createEmergencyVehicles();
    void spawnCivilianVehicle();
    void createEmergency(EmergencyType type, Position pos);
    void assignVehicleToEmergency(Emergency* emergency);
    void drawRoadGrid() const;
    void drawBuildings() const;
    void drawEmergency(const Emergency& emerg) const;
    void drawUI() const;
};
#endif
