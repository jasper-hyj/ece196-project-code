#ifndef ESPWEBCONTROLLER_H
#define ESPWEBCONTROLLER_H

#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <WiFi.h>

#include <map>
#include <queue>
#include <utility>
#include <string>

#include "EventType.h"

inline constexpr const char* WIFI_SSID = "ESP32-Team3-Access-Point";
inline constexpr const char* WIFI_PASSWORD = "123456789";

class ESPWebController {
   public:
    ESPWebController();
    void begin();
    void update();

    void send(EventType type, std::function<void(JsonDocument&)> fill);

    void setOnInitCallback(std::function<bool()> callback) {
        onInit = callback;
    }

    void setOnSetupCallback(std::function<void(int, int)> callback) {
        onSetup = callback;
    }

    void setOnNewWaypointCallback(std::function<void(int, int)> callback) {
        onNewWaypoint = callback;
    }
    

   private:
    void handleClientRequest(WiFiClient& client);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);

    static ESPWebController* instance;

    WiFiServer server{80};           // Initialize with port 80
    WebSocketsServer webSocket{81};  // WebSocket on port 81

    std::queue<std::pair<int, int>> waypoints;
    std::function<void(int, int)> onNewWaypoint = nullptr;  // callback for main loop
    std::function<bool()> onInit;
    std::function<void(int, int)> onSetup;
};

#endif
