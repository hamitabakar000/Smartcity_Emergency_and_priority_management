#ifndef VEHICLE_H
#define VEHICLE_H
#include "raylib.h"
#include "../core/position.h"
#include "../core/enums.h"
#include <vector>
class TrafficLight;
class RoadNetwork;
struct Emergency;
class Vehicle {
protected:
    Position position;
    Position homePosition;
    Direction direction;
    VehicleType type;
    VehicleStatus status;
    float speed;
    float baseSpeed;
    Color color;
    Emergency* assignedEmergency;
    float missionTimer;
    bool active;
    std::vector<Position> path;
    int currentPathIndex;
    float stoppedTimer;
    bool isStopped;
public:
    Vehicle(VehicleType t, Position home, Color col);
    virtual ~Vehicle();
    void assignMission(Emergency* emergency, const RoadNetwork& roadNet);
    void setPath(const std::vector<Position>& newPath);
    void update(float deltaTime, std::vector<TrafficLight*>& lights,
                const std::vector<Vehicle*>& vehicles, const RoadNetwork& roadNet);
    void draw() const;
    VehicleType getType() const { return type; }
    VehicleStatus getStatus() const { return status; }
    Position getPosition() const { return position; }
    bool isActive() const { return active; }
    Direction getDirection() const { return direction; }
private:
    void updateCivilian(float deltaTime, std::vector<TrafficLight*>& lights,
                       const std::vector<Vehicle*>& vehicles);
    void updateEmergency(float deltaTime, std::vector<TrafficLight*>& lights,
                        const std::vector<Vehicle*>& vehicles, const RoadNetwork& roadNet);
    void followPath();
    void move();
    void moveToSide();
    bool isSameDirection(Direction other) const;
    bool isSameOrOppositeDirection(Direction other) const;
};
#endif
