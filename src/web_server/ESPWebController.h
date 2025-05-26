#ifndef ESPWEBCONTROLLER_H
#define ESPWEBCONTROLLER_H

#include <WebSocketsServer.h>
#include <WiFi.h>

#include <map>

#include "../Direction.h"

#include <queue>
#include <utility>


class ESPWebController {
public:
    ESPWebController();
    void begin();
    void update();
    const std::map<Direction, bool>& getDirectionMap() const {
        return direction;
    }
    
    void broadcastPosition(double x, double y);

    void setOnNewWaypointCallback(std::function<void(int, int)> cb) {
        onNewWaypoint = cb;
    }

private:
    void handleClientRequest(WiFiClient& client);
    void handleCommand(const String& cmd, const String& state);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);

    static ESPWebController* instance;

    std::map<Direction, bool> direction;

    const char* ssid = "ESP32-Team3-Access-Point";
    const char* password = "123456789";
    WiFiServer server{80};           // Initialize with port 80
    WebSocketsServer webSocket{81};  // WebSocket on port 81

    std::queue<std::pair<int, int>> waypoints;
    std::function<void(int, int)> onNewWaypoint = nullptr; // callback for main loop

};

#endif
