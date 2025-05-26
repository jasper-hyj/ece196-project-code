#include <Arduino.h>

#include "motor/AccelStepperController.h"
#include "web_server/ESPWebController.h"

static constexpr int LEFT_EN = 9;
static constexpr int LEFT_STEP = 8;
static constexpr int LEFT_DIR = 7;
static constexpr int RIGHT_EN = 12;
static constexpr int RIGHT_STEP = 11;
static constexpr int RIGHT_DIR = 10;

static int windowWidth = 1000;  // default value
static constexpr int windowHeight = 1000; // default value
static constexpr int botWidth = 279.39;

bool initialized = false;

ESPWebController controller;

AccelStepperController accelStepperController(
    LEFT_EN, LEFT_STEP, LEFT_DIR,
    RIGHT_EN, RIGHT_STEP, RIGHT_DIR,
    windowWidth, botWidth);

void setup() {
    Serial.begin(115200);
    controller.begin();

    controller.setOnInitCallback([](int width) {
        accelStepperController.begin(width);
        Serial.printf("Window width set to: %d mm\n", width);
        initialized = true;
    });

    controller.setOnNewWaypointCallback([](double x, double y) {
        accelStepperController.enqueueTarget(x, y);
    });
}

void loop() {
    controller.update();

    if (initialized) {
        if (!accelStepperController.isMoving()) {
            accelStepperController.next();
        }
        accelStepperController.updateMovement();

        controller.broadcastInfo(accelStepperController.toJSON());

        Serial.printf("%s\n", accelStepperController.toString());
    }
}
