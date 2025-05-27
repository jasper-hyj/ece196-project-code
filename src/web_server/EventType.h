#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

#include <string>

enum class EventType {
    INIT,
    SETUP,
    WAYPOINT,
    INFO,
    UNKNOWN
};

const char* toString(EventType direction);
EventType getEventTypeEnum(const std::string& str);

#endif