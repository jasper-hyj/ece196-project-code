#ifndef DIRECTION_H
#define DIRECTION_H

#include <string>

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UNKNOWN
};

const char* toString(Direction direction);
Direction getDirectionEnum(const std::string& str);

#endif
