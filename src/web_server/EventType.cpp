#include "EventType.h"
#include <map>
#include <string>

const char* toString(EventType direction) {
    switch (direction) {

        case EventType::INIT: return "init";
        case EventType::WAYPOINT: return "waypoint";
        case EventType::INFO: return "info";
        case EventType::UNKNOWN: return "unknown";
        default: return "invalid";
    }
}

std::map<std::string, EventType> eventTypeDict = {
    {"init", EventType::INIT},
    {"waypoint", EventType::WAYPOINT},
    {"info", EventType::INFO}
};

EventType getEventTypeEnum(const std::string& str) {
    auto it = eventTypeDict.find(str);
    return (it != eventTypeDict.end()) ? it->second : EventType::UNKNOWN;
}
