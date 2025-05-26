#include "ESPWebController.h"

#include "index_html.h"

ESPWebController* ESPWebController::instance = nullptr;

ESPWebController::ESPWebController() {
    instance = this;
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

void ESPWebController::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_TEXT && instance) {
        String json = (char*)payload;

        JsonDocument doc;
        deserializeJson(doc, json);
        EventType eventType = getEventTypeEnum(doc["type"]);

        switch (eventType) {
            case EventType::INIT: {
                int width = doc["width"];
                Serial.printf("Received window width: %d mm\n", width);
                if (instance->onInit) {
                    instance->onInit(width);
                }
                break;
            }

            case EventType::WAYPOINT: {
                int x = doc["x"];
                int y = doc["y"];
                Serial.printf("Target received: x=%d, y=%d\n", x, y);
                if (instance->onNewWaypoint) {
                    instance->onNewWaypoint(x, y);
                }
            }

            case EventType::UNKNOWN: {
                break;
            }
        }
    }
}

void ESPWebController::broadcastInfo(JsonDocument infoJSON) {
    String output;
    serializeJson(infoJSON, output);
    webSocket.broadcastTXT(output);
}
