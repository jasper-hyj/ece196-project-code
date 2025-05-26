#ifndef ESPWEBCONTROLLER_H
#define ESPWEBCONTROLLER_H

#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <WiFi.h>

#include <map>
#include <queue>
#include <utility>

#include "EventType.h"

class ESPWebController {
   public:
    ESPWebController();
    void begin();
    void update();

    void broadcastInfo(JsonDocument infoJSON);

    void setOnNewWaypointCallback(std::function<void(int, int)> cb) {
        onNewWaypoint = cb;
    }
    void setOnInitCallback(std::function<void(int)> callback) {
        onInit = callback;
    }

   private:
    void handleClientRequest(WiFiClient& client);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);

    static ESPWebController* instance;

    const char* ssid = "ESP32-Team3-Access-Point";
    const char* password = "123456789";
    WiFiServer server{80};           // Initialize with port 80
    WebSocketsServer webSocket{81};  // WebSocket on port 81

    std::queue<std::pair<int, int>> waypoints;
    std::function<void(int, int)> onNewWaypoint = nullptr;  // callback for main loop
    std::function<void(int)> onInit;
};

#endif
