#include "infrastructure/traffic_light.h"
#include "core/constants.h"
#include <algorithm>
TrafficLight::TrafficLight(float x, float y) : position(x, y), 
    verticalState(LIGHT_GREEN), horizontalState(LIGHT_RED),
    timer(0), phase(0), emergencyOverride(false), overrideTimer(0) {}
void TrafficLight::update(float deltaTime) {
    if (emergencyOverride) {
        overrideTimer += deltaTime;
        if (overrideTimer > 4.0f) {
            emergencyOverride = false;
            overrideTimer = 0;
        }
        return;
    }
    timer += deltaTime;
    switch(phase) {
        case 0:
            if (timer > 6.0f) {
                verticalState = LIGHT_YELLOW;
                phase = 1;
                timer = 0;
            }
            break;
        case 1:
            if (timer > 2.0f) {
                verticalState = LIGHT_RED;
                horizontalState = LIGHT_GREEN;
                phase = 2;
                timer = 0;
            }
            break;
        case 2:
            if (timer > 6.0f) {
                horizontalState = LIGHT_YELLOW;
                phase = 3;
                timer = 0;
            }
            break;
        case 3:
            if (timer > 2.0f) {
                horizontalState = LIGHT_RED;
                verticalState = LIGHT_GREEN;
                phase = 0;
                timer = 0;
            }
            break;
    }
}
void TrafficLight::triggerEmergency(Direction emergencyDir) {
    emergencyOverride = true;
    overrideTimer = 0;
    if (emergencyDir == DIR_NORTH || emergencyDir == DIR_SOUTH) {
        verticalState = LIGHT_GREEN;
        horizontalState = LIGHT_RED;
    } else {
        horizontalState = LIGHT_GREEN;
        verticalState = LIGHT_RED;
    }
}
void TrafficLight::draw() const {
    float offsets[4][2] = {{-30, -30}, {30, -30}, {-30, 30}, {30, 30}};
    for (int i = 0; i < 4; i++) {
        float x = position.x + offsets[i][0];
        float y = position.y + offsets[i][1];
        DrawRectangle(x - 10, y - 24, 20, 48, BLACK);
        LightState state = (i < 2) ? verticalState : horizontalState;
        Color red = (state == LIGHT_RED) ? RED : Color{80, 0, 0, 255};
        Color yellow = (state == LIGHT_YELLOW) ? YELLOW : Color{80, 80, 0, 255};
        Color green = (state == LIGHT_GREEN) ? LIME : Color{0, 80, 0, 255};
        DrawCircle(x, y - 12, 6, red);
        DrawCircle(x, y, 6, yellow);
        DrawCircle(x, y + 12, 6, green);
    }
    if (emergencyOverride) {
        DrawCircleLines(position.x, position.y, 40, ORANGE);
        DrawCircleLines(position.x, position.y, 45, ORANGE);
    }
}
LightState TrafficLight::getState(Direction dir) const {
    return (dir == DIR_NORTH || dir == DIR_SOUTH) ? verticalState : horizontalState;
}
