#include "ESPWebController.h"

#include <ArduinoJson.h>

#include "index_html.h"

ESPWebController* ESPWebController::instance = nullptr;

ESPWebController::ESPWebController() {
    instance = this;

    direction[Direction::UP] = false;
    direction[Direction::DOWN] = false;
    direction[Direction::LEFT] = false;
    direction[Direction::RIGHT] = false;
}

void ESPWebController::begin() {
    WiFi.softAP(ssid, password);
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());

    server.begin();
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void ESPWebController::update() {
    webSocket.loop();

    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client.");
        handleClientRequest(client);
        client.stop();
        Serial.println("Client disconnected.");
    }
}

void ESPWebController::handleClientRequest(WiFiClient& client) {
    String request = client.readStringUntil('\r');
    client.read();  // consume \n

    if (request.indexOf("GET / ") >= 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.println(index_html);
        client.println();
    }
}

void ESPWebController::handleCommand(const String& cmd, const String& state) {
    // Serial.printf("Command: %s | State: %s\n", cmd.c_str(), state.c_str());
    bool boolState = (strcmp(state.c_str(), "press") == 0);

    switch (getDirectionEnum(cmd.c_str())) {
        case Direction::UP: {
            this->direction[Direction::UP] = boolState;
            break;
        }
        case Direction::DOWN: {
            this->direction[Direction::DOWN] = boolState;
            break;
        }
        case Direction::LEFT: {
            this->direction[Direction::LEFT] = boolState;
            break;
        }
        case Direction::RIGHT: {
            this->direction[Direction::RIGHT] = boolState;
            break;
        }
        default: {
            break;
        }
    }
}

void ESPWebController::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_TEXT && instance) {
        String json = (char*)payload;

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
            Serial.println("Failed to parse WebSocket JSON");
            return;
        }

        if (doc.containsKey("type") && doc["type"] == "init" && doc.containsKey("width")) {
            int width = doc["width"];
            Serial.printf("Received window width: %d mm\n", width);
            if (instance->onInitWidth) {
                instance->onInitWidth(width);
            }
            return;
        }

        if (doc.containsKey("x") && doc.containsKey("y")) {
            int x = doc["x"];
            int y = doc["y"];
            Serial.printf("Target received: x=%d, y=%d\n", x, y);
            if (instance->onNewWaypoint) {
                instance->onNewWaypoint(x, y);
            }
        }
    }
}

void ESPWebController::broadcastPosition(double x, double y) {
    JsonDocument doc;
    doc["x"] = (int)x;
    doc["y"] = (int)y;
    String output;
    serializeJson(doc, output);
    webSocket.broadcastTXT(output);
}

std::function<void(int)> onInitWidth;
void setOnInitWidthCallback(std::function<void(int)> callback) {
    onInitWidth = callback;
}
