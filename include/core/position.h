#ifndef POSITION_H
#define POSITION_H
#include "enums.h"
#include <cmath>
struct Position {
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
    float distanceTo(const Position& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    bool operator==(const Position& other) const {
        return std::abs(x - other.x) < 5.0f && std::abs(y - other.y) < 5.0f;
    }
};
struct RoadNode {
    Position position;
    int row, col;
    RoadNode(Position p, int r, int c) : position(p), row(r), col(c) {}
};
struct Emergency {
    Position position;
    EmergencyType type;
    bool active;
    bool assigned;
    float timer;
    int id;
    Emergency() : position(0, 0), type(EMERG_FIRE), active(false), assigned(false), timer(0), id(0) {}
};
#endif
