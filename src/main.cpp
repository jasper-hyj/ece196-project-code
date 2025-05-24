#include <Arduino.h>
#include "web_server/ESPWebController.h"
#include "motor/AccelStepperController.h"

static constexpr int LEFT_EN = 9;
static constexpr int LEFT_STEP = 8;
static constexpr int LEFT_DIR = 7;
static constexpr int RIGHT_EN = 12;
static constexpr int RIGHT_STEP = 11;
static constexpr int RIGHT_DIR = 10;

AccelStepperController accelStepperController(
    LEFT_EN, LEFT_STEP, LEFT_DIR,
    RIGHT_EN, RIGHT_STEP, RIGHT_DIR,
    10000, 279.39
);

ESPWebController controller;

void setup() {
    Serial.begin(115200);
    controller.begin();
    accelStepperController.begin();
}

void loop() {
    controller.update();
    const auto& directions = controller.getDirectionMap();

    for (const auto& pair : directions) {
        if (pair.second) {
            accelStepperController.move(pair.first);
            // Serial.printf("Position: (%f, %f)\n ", accelStepperController.getCurrentX(), accelStepperController.getCurrentY());
            Serial.printf("Rope Length: (%f, %f)\n ", accelStepperController.getCurrentLeftLength(), accelStepperController.getCurrentRightLength());
        }
    }
}
