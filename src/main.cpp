#include <Arduino.h>
#include "motor/TMC2209Pin.h"
#include "motor/L298NPin.h"
#include "motor/AccelStepperController.h"

#include "web_server/ESPWebController.h"

// Pin definition
constexpr TMC2209Pin leftStepperPin(9, 3, 2, 4, 6, 5, 8, 7);

constexpr TMC2209Pin rightStepperPin(17, 11, 10, 12,14, 13, 16, 15);

constexpr L298NPin midStepperPin(35, 33, 36, 48, 34, 47);

// Dimensions
constexpr int BOT_WIDTH = 261;
static int windowWidth = 1000;   // default value
static int windowHeight = 1000;  // default value

// Initialize tracking
bool initialized = false;

// Core components
ESPWebController webController;

AccelStepperController accelStepperController(
    leftStepperPin, rightStepperPin, midStepperPin,
    BOT_WIDTH);

void setup() {
    Serial.begin(115200);

    accelStepperController.begin();
    webController.begin();

    webController.setOnInitCallback([]() -> bool {
        return initialized;
    });

    webController.setOnSetupCallback([](int width, int height) {
        if (!initialized) {
            Serial.printf("main.cpp: Initializing: windowWidth=%d, windowHeight=%d\n", width, height);

            windowWidth = width;
            windowHeight = height;

            accelStepperController.setup(width, height);
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
