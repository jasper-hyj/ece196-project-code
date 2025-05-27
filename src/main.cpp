#include <Arduino.h>

#include "motor/AccelStepperController.h"
#include "web_server/ESPWebController.h"

static constexpr int LEFT_EN = 9;
static constexpr int LEFT_STEP = 8;
static constexpr int LEFT_DIR = 7;
static constexpr int RIGHT_EN = 12;
static constexpr int RIGHT_STEP = 11;
static constexpr int RIGHT_DIR = 10;

static int windowWidth = 1000;             // default value
static int windowHeight = 1000;             // default value
static constexpr int botWidth = 279.39;

unsigned long lastInitRequestTime = 0;
const unsigned long initRequestInterval = 2000;

bool initialized = false;

ESPWebController controller;

AccelStepperController accelStepperController(
    LEFT_EN, LEFT_STEP, LEFT_DIR,
    RIGHT_EN, RIGHT_STEP, RIGHT_DIR,
    botWidth);

void setup() {
    Serial.begin(115200);
    controller.begin();

    controller.setOnInitCallback([](int width, int height) {
        if (!initialized) {
            Serial.printf("main.cpp: Initializing: windowWidth=%d\n", width);

            windowWidth = width;
            windowHeight = height;

            accelStepperController.begin(width);
            initialized = true;
        }
    });

    controller.setOnNewWaypointCallback([](double x, double y) {
        accelStepperController.enqueueWaypoint(x, y);
    });
}

void loop() {
    controller.update();
    JsonDocument json;

    // Check if initialized
    if (!initialized) {
        unsigned long now = millis();
        if (now - lastInitRequestTime >= initRequestInterval) {
            Serial.printf("main.cpp: Request Init\n");
            controller.send(EventType::INIT, &json);
            lastInitRequestTime = now;
        }
        return;
    }

    // Include basic information
    json["botWidth"] = botWidth;
    json["windowWidth"] = windowWidth;
    json["windowHeight"] = windowHeight;

    // AccelStepperController 
    if (!accelStepperController.isMoving()) {
        accelStepperController.next();
    }
    accelStepperController.updateMovement();
    accelStepperController.toJSON(json);

    // Send information to frontend
    controller.send(EventType::INFO, &json);
}
