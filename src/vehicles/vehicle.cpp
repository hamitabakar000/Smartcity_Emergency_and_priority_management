#include "vehicles/vehicle.h"
#include "infrastructure/traffic_light.h"
#include "infrastructure/road_network.h"
#include "core/constants.h"
#include "core/enums.h"
#include "core/position.h"
#include <algorithm>
#include <cmath>
Vehicle::Vehicle(VehicleType t, Position home, Color col) 
    : position(home), homePosition(home), direction(DIR_SOUTH),
      type(t), status(STATUS_IDLE), color(col),
      assignedEmergency(nullptr), missionTimer(0), active(true),
      currentPathIndex(0), stoppedTimer(0), isStopped(false) {
    switch(t) {
        case TYPE_AMBULANCE:
            baseSpeed = 3.5f;
            break;
        case TYPE_POLICE:
            baseSpeed = 4.0f;
            break;
        case TYPE_FIRE_TRUCK:
            baseSpeed = 3.2f;
            break;
        case TYPE_CIVILIAN:
            baseSpeed = 2.2f + (float)(GetRandomValue(0, 8)) / 10.0f;
            break;
    }
    speed = baseSpeed;
}
Vehicle::~Vehicle() {}
void Vehicle::assignMission(Emergency* emergency, const RoadNetwork& roadNet) {
    if (emergency && type != TYPE_CIVILIAN) {
        assignedEmergency = emergency;
        status = STATUS_EN_ROUTE;
        emergency->assigned = true;
        missionTimer = 0;
        path = roadNet.findPath(position, emergency->position);
        currentPathIndex = 0;
    }
}
void Vehicle::setPath(const std::vector<Position>& newPath) {
    path = newPath;
    currentPathIndex = 0;
}
void Vehicle::update(float deltaTime, std::vector<TrafficLight*>& lights,
                    const std::vector<Vehicle*>& vehicles, const RoadNetwork& roadNet) {
    if (type == TYPE_CIVILIAN) {
        updateCivilian(deltaTime, lights, vehicles);
    } else {
        updateEmergency(deltaTime, lights, vehicles, roadNet);
    }
}
void Vehicle::updateCivilian(float deltaTime, std::vector<TrafficLight*>& lights,
                           const std::vector<Vehicle*>& vehicles) {
    bool emergencyNearby = false;
    Direction emergencyDir = DIR_NORTH;
    for (auto v : vehicles) {
        if (v->type != TYPE_CIVILIAN && v->status == STATUS_EN_ROUTE) {
            float dist = position.distanceTo(v->position);
            if (dist < 120) {
                emergencyNearby = true;
                emergencyDir = v->direction;
                if (isSameOrOppositeDirection(v->direction)) {
                    moveToSide();
                    isStopped = true;
                    stoppedTimer = 0;
                    return;
                }
            }
        }
    }
    if (isStopped) {
        stoppedTimer += deltaTime;
        if (stoppedTimer > 2.0f) {
            isStopped = false;
            stoppedTimer = 0;
        } else {
            return;
        }
    }
    bool shouldStopAtLight = false;
    for (auto light : lights) {
        float dist = position.distanceTo(light->getPosition());
        if (dist < 70 && dist > 30) {
            LightState state = light->getState(direction);
            if (state == LIGHT_RED || state == LIGHT_YELLOW) {
                shouldStopAtLight = true;
                break;
            }
        }
    }
    if (shouldStopAtLight) {
        return;
    }
    bool vehicleAhead = false;
    for (auto v : vehicles) {
        if (v == this) continue;
        float dist = position.distanceTo(v->position);
        if (dist < 45 && isSameDirection(v->direction)) {
            bool ahead = false;
            switch(direction) {
                case DIR_NORTH: ahead = (v->position.y < position.y); break;
                case DIR_SOUTH: ahead = (v->position.y > position.y); break;
                case DIR_EAST: ahead = (v->position.x > position.x); break;
                case DIR_WEST: ahead = (v->position.x < position.x); break;
            }
            if (ahead) {
                vehicleAhead = true;
                speed = std::min(speed, v->speed * 0.9f);
                break;
            }
        }
    }
    if (!vehicleAhead) {
        speed = baseSpeed;
    }
    move();
    if (position.x < -100 || position.x > WINDOW_WIDTH + 100 ||
        position.y < -100 || position.y > WINDOW_HEIGHT + 100) {
        active = false;
    }
}
void Vehicle::moveToSide() {
    float sideOffset = 15;
    if (direction == DIR_NORTH || direction == DIR_SOUTH) {
        position.x += sideOffset;
    } else {
        position.y += sideOffset;
    }
}
bool Vehicle::isSameOrOppositeDirection(Direction other) const {
    if (direction == DIR_NORTH && other == DIR_NORTH) return true;
    if (direction == DIR_SOUTH && other == DIR_SOUTH) return true;
    if (direction == DIR_NORTH && other == DIR_SOUTH) return true;
    if (direction == DIR_SOUTH && other == DIR_NORTH) return true;
    if (direction == DIR_EAST && other == DIR_EAST) return true;
    if (direction == DIR_WEST && other == DIR_WEST) return true;
    if (direction == DIR_EAST && other == DIR_WEST) return true;
    if (direction == DIR_WEST && other == DIR_EAST) return true;
    return false;
}
void Vehicle::updateEmergency(float deltaTime, std::vector<TrafficLight*>& lights,
                            const std::vector<Vehicle*>& vehicles, const RoadNetwork& roadNet) {
    if (status == STATUS_IDLE) return;
    for (auto light : lights) {
        float dist = position.distanceTo(light->getPosition());
        if (dist < 150 && status == STATUS_EN_ROUTE) {
            light->triggerEmergency(direction);
        }
    }
    if (status == STATUS_EN_ROUTE && assignedEmergency) {
        followPath();
        if (position.distanceTo(assignedEmergency->position) < 35) {
            status = STATUS_AT_SCENE;
            missionTimer = 0;
            speed = 0;
        }
    } else if (status == STATUS_AT_SCENE) {
        missionTimer += deltaTime;
        if (missionTimer > 5.0f) {
            assignedEmergency->active = false;
            assignedEmergency = nullptr;
            status = STATUS_RETURNING;
            speed = baseSpeed;
            path = roadNet.findPath(position, homePosition);
            currentPathIndex = 0;
        }
    } else if (status == STATUS_RETURNING) {
        for (auto light : lights) {
            float dist = position.distanceTo(light->getPosition());
            if (dist < 150) {
                light->triggerEmergency(direction);
            }
        }
        followPath();
        if (position.distanceTo(homePosition) < 30) {
            position = homePosition;
            status = STATUS_IDLE;
            direction = DIR_SOUTH;
            speed = 0;
            path.clear();
            currentPathIndex = 0;
        }
    }
}
void Vehicle::followPath() {
    if (path.empty() || currentPathIndex >= path.size()) {
        return;
    }
    Position target = path[currentPathIndex];
    float dist = position.distanceTo(target);
    if (dist < 10.0f) {
        currentPathIndex++;
        if (currentPathIndex < path.size()) {
            target = path[currentPathIndex];
        } else {
            return;
        }
    }
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float totalDist = std::sqrt(dx * dx + dy * dy);
    if (totalDist < 1.0f) return;
    if (std::abs(dx) > std::abs(dy)) {
        direction = (dx > 0) ? DIR_EAST : DIR_WEST;
    } else {
        direction = (dy > 0) ? DIR_SOUTH : DIR_NORTH;
    }
    float speedMultiplier = (status == STATUS_EN_ROUTE) ? 1.5f : 1.2f;
    float actualSpeed = speed * speedMultiplier;
    position.x += (dx / totalDist) * actualSpeed;
    position.y += (dy / totalDist) * actualSpeed;
}
void Vehicle::move() {
    switch(direction) {
        case DIR_NORTH: position.y -= speed; break;
        case DIR_SOUTH: position.y += speed; break;
        case DIR_EAST: position.x += speed; break;
        case DIR_WEST: position.x -= speed; break;
    }
}
bool Vehicle::isSameDirection(Direction other) const {
    return direction == other;
}
void Vehicle::draw() const {
    float w = (direction == DIR_NORTH || direction == DIR_SOUTH) ? 28 : 50;
    float h = (direction == DIR_NORTH || direction == DIR_SOUTH) ? 50 : 28;
    DrawRectangle(position.x - w/2, position.y - h/2, w, h, color);
    DrawRectangleLines(position.x - w/2, position.y - h/2, w, h, BLACK);
    Color windowColor = Color{100, 150, 200, 200};
    if (direction == DIR_NORTH || direction == DIR_SOUTH) {
        DrawRectangle(position.x - w/2 + 4, position.y - h/4, w - 8, h/3, windowColor);
    } else {
        DrawRectangle(position.x - w/4, position.y - h/2 + 4, w/3, h - 8, windowColor);
    }
    if (type != TYPE_CIVILIAN) {
        if (status == STATUS_EN_ROUTE || status == STATUS_RETURNING) {
            int flash = ((int)(GetTime() * 8) % 2);
            if (flash == 0) {
                DrawCircle(position.x - 8, position.y - h/2 - 8, 5, RED);
                DrawCircle(position.x + 8, position.y - h/2 - 8, 5, BLUE);
            } else {
                DrawCircle(position.x - 8, position.y - h/2 - 8, 5, BLUE);
                DrawCircle(position.x + 8, position.y - h/2 - 8, 5, RED);
            }
            DrawCircleLines(position.x, position.y, 40, Fade(ORANGE, 0.6f));
            DrawCircleLines(position.x, position.y, 50, Fade(YELLOW, 0.4f));
        } else if (status == STATUS_AT_SCENE) {
            DrawCircle(position.x - 8, position.y - h/2 - 8, 5, RED);
            DrawCircle(position.x + 8, position.y - h/2 - 8, 5, BLUE);
        }
    }
}
