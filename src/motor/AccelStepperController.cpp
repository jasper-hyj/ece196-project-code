#include "AccelStepperController.h"

#include <algorithm>
#include <cmath>

AccelStepperController* AccelStepperController::instance = nullptr;

AccelStepperController::AccelStepperController(
    int leftEn, int leftStep, int leftDir,
    int rightEn, int rightStep, int rightDir, double botWidth
    ) : stepperLeft(AccelStepper::DRIVER, leftStep, leftDir),
      stepperRight(AccelStepper::DRIVER, rightStep, rightDir),
      leftEn(leftEn),
      rightEn(rightEn),
      botWidth(botWidth) {
    instance = this;
}

void AccelStepperController::begin(int windowWidth) {
    // Enable motors
    pinMode(leftEn, OUTPUT);
    pinMode(rightEn, OUTPUT);

    digitalWrite(leftEn, LOW);
    digitalWrite(rightEn, LOW);

    // Calculate the correct X: Top left corner is (0,0)
    currentX = botWidth / 2.0;

    // Calculate the right rope length: Assume start top left
    currentRight = windowWidth - botWidth;

    // Stepper setup
    stepperLeft.setMaxSpeed(MAX_SPEED);
    stepperLeft.setAcceleration(ACCEL);
    stepperLeft.setCurrentPosition(currentLeft * STEPS_PER_MM);

    stepperRight.setMaxSpeed(MAX_SPEED);
    stepperRight.setAcceleration(ACCEL);
    stepperRight.setCurrentPosition(currentRight * STEPS_PER_MM);

    this->windowWidth = windowWidth;
}

void AccelStepperController::enqueueWaypoint(double x, double y) {
    Serial.printf("AccelStepperController.cpp: Enqueued Target: x=%d, y=%d\n", x, y);
    waypoints.push(std::pair<int, int>(x, y));
}

void AccelStepperController::next() {
    if (waypoints.empty()) return;

    auto target = waypoints.front();

    double x = target.first;
    double y = target.second;

    if (x < (botWidth / 2.0) || x > (windowWidth - botWidth / 2.0)) {
        x = currentX;
    }
    if (y < 0) {
        y = currentY;
    }

    targetX = x;
    targetY = y;
    lastLeft = currentLeft;
    lastRight = currentRight;
    leftMoveTo = currentLeft;
    rightMoveTo = currentRight;
    moveToX = currentX;
    moveToY = currentY;

    double diffX = targetX - currentX;
    double diffY = targetY - currentY;

    double distance = std::sqrt(diffX * diffX + diffY * diffY);
    if (distance == 0) {
        moving = false;
        return;
    }
    double scale = MAX_MM_CHANGE / distance;
    xChange = diffX * scale;
    yChange = diffY * scale;

    moving = true;
}

void AccelStepperController::updateMovement() {
    if (!moving) return;

    // Find current left/right length
    currentLeft = stepperLeft.currentPosition() / STEPS_PER_MM;
    currentRight = stepperRight.currentPosition() / STEPS_PER_MM;

    // Find current left/right coordinate
    double xLeft = (std::pow(currentLeft, 2) - std::pow(currentRight, 2) + std::pow(windowWidth - botWidth, 2)) / (2 * (windowWidth - botWidth));
    currentX = xLeft + botWidth / 2.0;
    // Clamp to prevent sqrt of negative due to rounding errors
    double ySquared = std::pow(currentLeft, 2) - std::pow(xLeft, 2);
    currentY = std::sqrt(std::max(0.0, ySquared));

    if (std::abs(stepperLeft.distanceToGo()) <= 1 && std::abs(stepperRight.distanceToGo()) <= 1) {  // If the moveTo haven't set to the final x
        if (moveToX != targetX || moveToY != targetY) {
            // Update moveToX based on nextX
            double nextX = moveToX + xChange;
            if ((xChange > 0 && nextX > targetX) || (xChange < 0 && nextX < targetX)) {
                moveToX = targetX;
            } else {
                moveToX = nextX;
            }

            // Update moveToY based on nextY
            double nextY = moveToY + yChange;
            if ((yChange > 0 && nextY > targetY) || (yChange < 0 && nextY < targetY)) {
                moveToY = targetY;
            } else {
                moveToY = nextY;
            }

            double leftMoveToX = moveToX - (botWidth / 2.0);   // x for left motor
            double rightMoveToX = moveToX + (botWidth / 2.0);  // x for right motor

            lastLeft = leftMoveTo;
            lastRight = rightMoveTo;

            leftMoveTo = std::sqrt(std::pow(leftMoveToX, 2) + std::pow(moveToY, 2));
            rightMoveTo = std::sqrt(std::pow(windowWidth - rightMoveToX, 2) + std::pow(moveToY, 2));

            stepperLeft.moveTo(leftMoveTo * STEPS_PER_MM);
            stepperRight.moveTo(rightMoveTo * STEPS_PER_MM);
        }
    }

    if (moveToX == targetX && moveToY == targetY &&
        std::abs(stepperLeft.distanceToGo()) <= 1 &&
        std::abs(stepperRight.distanceToGo()) <= 1) {
        waypoints.pop();
        moving = false;
        return;
    }

    double deltaLeft = std::abs(leftMoveTo - lastLeft);
    double deltaRight = std::abs(rightMoveTo - lastRight);
    double maxDelta = std::max(deltaLeft, deltaRight);

    if (maxDelta == 0) {
        stepperLeft.setSpeed(0);
        stepperRight.setSpeed(0);
    } else {
        double leftSpeed = (deltaLeft / maxDelta) * MAX_SPEED;
        double rightSpeed = (deltaRight / maxDelta) * MAX_SPEED;

        double leftDir = (leftMoveTo >= lastLeft) ? 1.0 : -1.0;
        double rightDir = (rightMoveTo >= lastRight) ? 1.0 : -1.0;

        leftMotorSpeed = leftSpeed * leftDir;
        rightMotorSpeed = rightSpeed * rightDir;

        stepperLeft.setSpeed(leftMotorSpeed);
        stepperRight.setSpeed(rightMotorSpeed);
    }

    stepperLeft.runSpeedToPosition();
    stepperRight.runSpeedToPosition();
}

void AccelStepperController::stop() {
    moving = false;
    stepperLeft.stop();
    stepperRight.stop();
}
