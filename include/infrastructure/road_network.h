#ifndef ROAD_NETWORK_H
#define ROAD_NETWORK_H
#include <vector>
#include <cfloat>
#include "../core/position.h"
class RoadNetwork {
private:
    std::vector<RoadNode> nodes;
public:
    RoadNetwork();
    Position getNearestRoadNode(Position pos) const;
    std::vector<Position> findPath(Position start, Position end) const;
};
#endif
