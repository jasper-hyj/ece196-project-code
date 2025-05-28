#include <Arduino.h>

#include "motor/AccelStepperController.h"
#include "web_server/ESPWebController.h"

// Pin definition
constexpr int LEFT_EN = 9;
constexpr int LEFT_STEP = 8;
constexpr int LEFT_DIR = 7;

constexpr int RIGHT_EN = 12;
constexpr int RIGHT_STEP = 11;
constexpr int RIGHT_DIR = 10;

constexpr int MID_IN1 = 36;
constexpr int MID_IN2 = 48;
constexpr int MID_IN3 = 34;
constexpr int MID_IN4 = 47;
constexpr int MID_ENA = 35;
constexpr int MID_ENB = 33;

// Dimensions
constexpr int BOT_WIDTH = 279.39;
static int windowWidth = 1000;   // default value
static int windowHeight = 1000;  // default value

// Initialize tracking
bool initialized = false;

// Core components
ESPWebController webController;

AccelStepperController accelStepperController(
    LEFT_EN, LEFT_STEP, LEFT_DIR,
    RIGHT_EN, RIGHT_STEP, RIGHT_DIR,
    MID_IN1, MID_IN2, MID_IN3, MID_IN4, MID_ENA, MID_ENB,
    BOT_WIDTH);

void setup() {
    Serial.begin(115200);

    webController.begin();
    accelStepperController.begin();

    webController.setOnInitCallback([]() -> bool {
        return initialized;
    });

    webController.setOnSetupCallback([](int width, int height) {
        if (!initialized) {
            Serial.printf("main.cpp: Initializing: windowWidth=%d, windowHeight=%d\n", width, height);

            windowWidth = width;
            windowHeight = height;

            accelStepperController.setup(width);
            initialized = true;
        }
    });

    webController.setOnNewWaypointCallback([](int x, int y) {
        accelStepperController.enqueueWaypoint(x, y);
    });
}

void loop() {
    webController.update();
    JsonDocument json;

    // Check if initialized
    if (!initialized) {
        return;
    }

    // Stepper Logic
    if (!accelStepperController.isMoving()) {
        accelStepperController.next();
    }
    accelStepperController.updateMovement();
    accelStepperController.spinMid();

    // Send information to frontend
    webController.send(EventType::INFO, [&](JsonDocument& json) {
        json["botWidth"] = BOT_WIDTH;
        json["windowWidth"] = windowWidth;
        json["windowHeight"] = windowHeight;
        accelStepperController.toJSON(json);
    });
}
