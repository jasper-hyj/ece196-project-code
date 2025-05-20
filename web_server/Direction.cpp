#include "Direction.h"
#include <map>
#include <string>

const char* toString(Direction direction) {
    switch (direction) {
        case Direction::UP: return "UP";
        case Direction::DOWN: return "DOWN";
        case Direction::LEFT: return "LEFT";
        case Direction::RIGHT: return "RIGHT";
        case Direction::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

std::map<std::string, Direction> directionDict = {
    {"up", Direction::UP},
    {"down", Direction::DOWN},
    {"left", Direction::LEFT},
    {"right", Direction::RIGHT}
};

Direction getDirectionEnum(const std::string& str) {
    auto it = directionDict.find(str);
    return (it != directionDict.end()) ? it->second : Direction::UNKNOWN;
}
