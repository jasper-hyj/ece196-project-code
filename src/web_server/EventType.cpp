#include "EventType.h"
#include <map>
#include <string>

const char* toString(EventType direction) {
    switch (direction) {

        case EventType::INIT: return "INIT";
        case EventType::WAYPOINT: return "WAYPOINT";
        case EventType::INFO: return "INFO";
        case EventType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
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
