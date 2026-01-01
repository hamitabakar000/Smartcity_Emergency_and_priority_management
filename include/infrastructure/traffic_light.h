#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H
#include "raylib.h"
#include "../core/position.h"
#include "../core/enums.h"
class TrafficLight {
private:
    Position position;
    LightState verticalState;
    LightState horizontalState;
    float timer;
    int phase;
    bool emergencyOverride;
    float overrideTimer;
public:
    TrafficLight(float x, float y);
    void update(float deltaTime);
    void triggerEmergency(Direction emergencyDir);
    void draw() const;
    LightState getState(Direction dir) const;
    Position getPosition() const { return position; }
    bool isEmergencyOverride() const { return emergencyOverride; }
};
#endif
