#include "ESPWebController.h"

#include <SPIFFS.h>

ESPWebController* ESPWebController::instance = nullptr;

ESPWebController::ESPWebController() {
    instance = this;
}

void ESPWebController::begin() {
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

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
    client.read();

    if (request.indexOf("GET / ") >= 0) {
        File file = SPIFFS.open("/index.html", "r");
        if (!file) {
            Serial.println("Failed to open /index.html");
            client.println("HTTP/1.1 500 Internal Server Error");
            client.println("Content-type:text/plain");
            client.println("Connection: close");
            client.println();
            client.println("Failed to load index.html");
            return;
        }

        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();

        client.write(file);

        while (file.available()) {
            client.write(file.read());
        }

        file.close();
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
                Serial.printf("ESPWebController.cpp:(EventType::INIT) Check init\n");

                if (instance->onInit) {
                    bool initialized = instance->onInit();
                    instance->send(EventType::INIT, [&](JsonDocument& doc) {
                        doc["initialized"] = initialized;
                    });
                }
                break;
            }
            case EventType::SETUP: {
                int windowWidth = doc["windowWidth"];
                int windowHeight = doc["windowHeight"];
                Serial.printf("ESPWebController.cpp:(EventType::SETUP) Received window width: %d mm, height: %d mm\n", windowWidth, windowHeight);
                if (instance->onSetup) {
                    instance->onSetup(windowWidth, windowHeight);
                }
                break;
            }

            case EventType::WAYPOINT: {
                int x = doc["x"];
                int y = doc["y"];
                Serial.printf("ESPWebController.cpp:(EventType::WAYPOINT) Target received: x=%d, y=%d\n", x, y);
                if (instance->onNewWaypoint) {
                    instance->onNewWaypoint(x, y);
                }
                break;
            }

            default:
                Serial.println("Received unknown event type");
                break;
        }
    }
}

void ESPWebController::send(EventType type, std::function<void(JsonDocument&)> fill) {
    JsonDocument doc;
    fill(doc);
    
    doc["type"] = toString(type);

    String output;
    serializeJson(doc, output);

    webSocket.broadcastTXT(output);
}