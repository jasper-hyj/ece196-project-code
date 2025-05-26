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
static constexpr int botWidth = 279.39;

ESPWebController controller;

std::queue<std::pair<int, int>> targets;

AccelStepperController accelStepperController(
    LEFT_EN, LEFT_STEP, LEFT_DIR,
    RIGHT_EN, RIGHT_STEP, RIGHT_DIR,
    windowWidth, botWidth);

void setup() {
    Serial.begin(115200);
    controller.begin();
    accelStepperController.begin();

    controller.setOnInitWidthCallback([](int width) {
        windowWidth = constrain(width, botWidth + 1, 3000); // set and bound
        accelStepperController.setWindowWidth(windowWidth);
        Serial.printf("Window width set to: %d mm\n", windowWidth);
    });

    controller.setOnNewWaypointCallback([](int xPercent, int yPercent) {
        float actualX = constrain((xPercent / 100.0f) * windowWidth, botWidth / 2.0f, windowWidth - botWidth / 2.0f);
        float actualY = constrain((yPercent / 100.0f) * windowHeight, 0.0f, windowHeight);
        accelStepperController.enqueueTarget(actualX, actualY);
    });
}

void loop() {
    controller.update();

    if (!accelStepperController.isMoving() && !targets.empty()) {
        auto target = targets.front();
        targets.pop();
        accelStepperController.moveToPosition(target.first, target.second);
    }

    accelStepperController.updateMovement();
    float relativeX = constrain((accelStepperController.getCurrentX() + botWidth / 2.0) / (windowWidth), 0.0, 1.0) * 100;
    float relativeY = constrain((accelStepperController.getCurrentY() + botWidth / 2.0) / (windowHeight), 0.0, 1.0) * 100;
    controller.broadcastPosition(relativeX, relativeY);

    Serial.printf("%s\n", accelStepperController.toString());
}
