#include "AccelStepperController.h"

#include <algorithm>
#include <cmath>

AccelStepperController* AccelStepperController::instance = nullptr;

AccelStepperController::AccelStepperController(
    int leftEn, int leftStep, int leftDir,
    int rightEn, int rightStep, int rightDir,
    int midIn1, int midIn2, int midIn3, int midIn4, int midEnA, int midEnB,
    double botWidth)
    : stepperLeft(AccelStepper::DRIVER, leftStep, leftDir),
      stepperRight(AccelStepper::DRIVER, rightStep, rightDir),
      stepperMid(AccelStepper::FULL4WIRE, midIn1, midIn2, midIn3, midIn4),
      leftEn(leftEn), rightEn(rightEn), midEnA(midEnA), midEnB(midEnB),
      botWidth(botWidth) {
    instance = this;

    stepperLeft.setEnablePin(leftEn);
    stepperRight.setEnablePin(rightEn);

    
}

void AccelStepperController::begin(int windowWidth) {
    // Enable motors
    pinMode(leftEn, OUTPUT);
    pinMode(rightEn, OUTPUT);

    digitalWrite(leftEn, LOW);
    digitalWrite(rightEn, LOW);

    analogWrite(midEnA, 255);
    analogWrite(midEnB, 255);

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
    stepperRight.setCurrentPosition((-1) * currentRight * STEPS_PER_MM);

    stepperMid.setMaxSpeed(MAX_SPEED);
    stepperMid.setAcceleration(ACCEL);
    stepperMid.setCurrentPosition(0);

    this->windowWidth = windowWidth;
}

void AccelStepperController::enqueueWaypoint(int x, int y) {
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
    currentRight = (-1) * stepperRight.currentPosition() / STEPS_PER_MM;

    // Find current left/right coordinate
    double xLeft = (std::pow(currentLeft, 2) - std::pow(currentRight, 2) + std::pow(windowWidth - botWidth, 2)) / (2 * (windowWidth - botWidth));
    currentX = xLeft + botWidth / 2.0;
    // Clamp to prevent sqrt of negative due to rounding errors
    double ySquared = std::pow(currentLeft, 2) - std::pow(xLeft, 2);
    currentY = std::sqrt(std::max(0.0, ySquared));

    double distanceToGo = std::sqrt(std::pow(stepperLeft.distanceToGo(), 2) + std::pow(stepperRight.distanceToGo(), 2)) / STEPS_PER_MM;
    if (distanceToGo <= RECALC_DIST) {
        // If the moveTo haven't set to the final x
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
            stepperRight.moveTo((-1) * rightMoveTo * STEPS_PER_MM);
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
    double sumDelta = deltaLeft + deltaRight;
    double maxDelta = std::max(deltaLeft, deltaRight);

    if (sumDelta == 0) {
        stepperLeft.setSpeed(0);
        stepperRight.setSpeed(0);
    } else {
        double leftSpeed = deltaLeft / (maxDelta / MAX_SPEED);
        double rightSpeed = deltaRight / (maxDelta / MAX_SPEED);

        double leftDir = (leftMoveTo >= lastLeft) ? 1.0 : -1.0;
        double rightDir = (rightMoveTo >= lastRight) ? -1.0 : 1.0;

        stepperLeft.setSpeed(leftSpeed * leftDir);
        stepperRight.setSpeed(rightSpeed * rightDir);
    }

    leftMotorSpeed = (std::abs(stepperLeft.distanceToGo()) > 1) ? stepperLeft.speed() / STEPS_PER_MM : 0;
    rightMotorSpeed = (std::abs(stepperRight.distanceToGo()) > 1) ? stepperRight.speed() / STEPS_PER_MM : 0;

    stepperLeft.runSpeedToPosition();
    stepperRight.runSpeedToPosition();
}

void AccelStepperController::spinMid() {
    currentMid = stepperMid.currentPosition() / STEPS_PER_MM;
    stepperMid.setSpeed((moving) ? MAX_SPEED : 0);

    midMotorSpeed = stepperMid.speed() / STEPS_PER_MM;

    stepperMid.runSpeed();
}

void AccelStepperController::stop() {
    moving = false;
    stepperLeft.stop();
    stepperRight.stop();
    stepperMid.stop();

    while (!waypoints.empty()) {
        waypoints.pop();
    }
}
