#include "AccelStepperController.h"

#include <cmath>

AccelStepperController* AccelStepperController::instance = nullptr;

AccelStepperController::AccelStepperController(
    int leftEn, int leftStep, int leftDir,
    int rightEn, int rightStep, int rightDir,
    double windowsWidth, double botWidth)
    : stepperLeft(AccelStepper::DRIVER, leftStep, leftDir),
      stepperRight(AccelStepper::DRIVER, rightStep, rightDir),
      leftEn(leftEn),
      rightEn(rightEn),
      windowsWidth(windowsWidth),
      botWidth(botWidth) {
    instance = this;

    currentX = botWidth / 2.0;
    rightLength = windowsWidth - botWidth;
}

void AccelStepperController::begin() {
    // Enable motors
    pinMode(leftEn, OUTPUT);
    pinMode(rightEn, OUTPUT);

    digitalWrite(leftEn, LOW);
    digitalWrite(rightEn, LOW);

    // Stepper setup
    stepperLeft.setMaxSpeed(MAX_SPEED);
    stepperLeft.setAcceleration(ACCEL);
    stepperLeft.setCurrentPosition(0);

    stepperRight.setMaxSpeed(MAX_SPEED);
    stepperRight.setAcceleration(ACCEL);
    stepperRight.setCurrentPosition(0);
}

double AccelStepperController::getCurrentX() {
    return currentX;
}

double AccelStepperController::getCurrentY() {
    return currentY;
}

double AccelStepperController::getCurrentLeftLength() {
    return leftLength;
}

double AccelStepperController::getCurrentRightLength() {
    return rightLength;
}

void AccelStepperController::move(Direction direction) {
    switch (direction) {
        case Direction::UP: {
            moveToPosition(currentX, currentY - 10);
            break;
        }
        case Direction::DOWN: {
            moveToPosition(currentX, currentY + 10);
            break;
        }
        case Direction::LEFT: {
            moveToPosition(currentX - 10, currentY);
            break;
        }
        case Direction::RIGHT: {
            moveToPosition(currentX + 10, currentY);
            break;
        }
        default: {
            break;
        }
    }
}

void AccelStepperController::moveToPosition(double x, double y) {

    if (x < (botWidth / 2.0) || x > (windowsWidth - botWidth / 2.0)) {
        x = currentX;
    }

    if (y < 0) {
        y = currentY;
    }

    // Calculate X location
    double leftX = x - (botWidth / 2.0);
    double rightX = x + (botWidth / 2.0);

    // Calculate left/right length
    double targetLeftLength = std::sqrt(std::pow(leftX, 2) + std::pow(y, 2));
    double targetRightLength = std::sqrt(std::pow(windowsWidth - rightX, 2) + std::pow(y, 2));

    
    Serial.printf("Left Length: (%f to %f)\n ", leftLength, targetLeftLength);
    Serial.printf("Right Length: (%f to %f)\n ", rightLength, targetRightLength);

    // Find the length difference
    long leftSteps = ((targetLeftLength - leftLength) / MM_PER_REV) * STEPS_PER_REV;
    long rightSteps = ((targetRightLength - rightLength) / MM_PER_REV) * STEPS_PER_REV;

    Serial.printf("Steps: (%d, %d)\n ", leftSteps, rightSteps);

    stepperLeft.setCurrentPosition(0);
    stepperRight.setCurrentPosition(0);
    stepperLeft.moveTo(leftSteps);
    stepperRight.moveTo(rightSteps);
    runBoth();

    leftLength = targetLeftLength;
    rightLength = targetRightLength;
    currentX = x;
    currentY = y;
}

void AccelStepperController::runBoth() {
    while (stepperLeft.distanceToGo() != 0 || stepperRight.distanceToGo() != 0) {
        stepperLeft.run();
        stepperRight.run();
    }
}
