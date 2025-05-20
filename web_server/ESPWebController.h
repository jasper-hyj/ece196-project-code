#ifndef ESPWEBCONTROLLER_H
#define ESPWEBCONTROLLER_H

#include "Direction.h"
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <map>

class ESPWebController {
public:
  ESPWebController();
  void begin();
  void update();
  const std::map<Direction, bool>& getDirectionMap() const {
    return direction;
  }

private:
  void handleClientRequest(WiFiClient &client);
  void handleCommand(const String& cmd, const String& state);
  static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

  static ESPWebController* instance;

  std::map<Direction, bool> direction;

  const char* ssid = "ESP32-Team3-Access-Point";
  const char* password = "123456789";
  WiFiServer server{80};           // Initialize with port 80
  WebSocketsServer webSocket{81};  // WebSocket on port 81
};

#endif
