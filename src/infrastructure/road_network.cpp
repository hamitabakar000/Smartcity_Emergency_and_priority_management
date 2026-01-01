#include "infrastructure/road_network.h"
#include "core/constants.h"
#include <cmath>
RoadNetwork::RoadNetwork() {
    for (int row = 0; row <= GRID_SIZE; row++) {
        for (int col = 0; col <= GRID_SIZE; col++) {
            float x = col * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
            float y = row * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
            nodes.push_back(RoadNode(Position(x, y), row, col));
        }
    }
}
Position RoadNetwork::getNearestRoadNode(Position pos) const {
    float minDist = FLT_MAX;
    Position nearest = nodes[0].position;
    for (const auto& node : nodes) {
        float dist = pos.distanceTo(node.position);
        if (dist < minDist) {
            minDist = dist;
            nearest = node.position;
        }
    }
    return nearest;
}
std::vector<Position> RoadNetwork::findPath(Position start, Position end) const {
    std::vector<Position> path;
    Position startNode = getNearestRoadNode(start);
    Position endNode = getNearestRoadNode(end);
    int startRow = -1, startCol = -1, endRow = -1, endCol = -1;
    for (const auto& node : nodes) {
        if (node.position == startNode) {
            startRow = node.row;
            startCol = node.col;
        }
        if (node.position == endNode) {
            endRow = node.row;
            endCol = node.col;
        }
    }
    if (startRow == -1 || endRow == -1) return path;
    path.push_back(startNode);
    int colStep = (endCol > startCol) ? 1 : -1;
    for (int col = startCol; col != endCol; col += colStep) {
        float x = (col + colStep) * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
        float y = startRow * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
        path.push_back(Position(x, y));
    }
    int rowStep = (endRow > startRow) ? 1 : -1;
    for (int row = startRow; row != endRow; row += rowStep) {
        float x = endCol * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
        float y = (row + rowStep) * (CELL_SIZE + ROAD_WIDTH) + ROAD_WIDTH/2;
        path.push_back(Position(x, y));
    }
    path.push_back(endNode);
    path.push_back(end);
    return path;
}
