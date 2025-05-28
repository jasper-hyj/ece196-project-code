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
      leftEn(leftEn),
      rightEn(rightEn),
      midEnA(midEnA),
      midEnB(midEnB),
      botWidth(botWidth) {
    instance = this;
}

void AccelStepperController::begin() {
    // Enable motors
    digitalWrite(leftEn, LOW);
    digitalWrite(rightEn, LOW);

    pinMode(midEnA, OUTPUT);
    pinMode(midEnB, OUTPUT);

    analogWrite(midEnA, 255);
    analogWrite(midEnB, 255);

    for (auto* stepper : {&stepperLeft, &stepperRight}) {
        stepper->setMaxSpeed(MAX_SPEED);
        stepper->setAcceleration(ACCEL);
        stepper->setMinPulseWidth(2);
    }

    stepperLeft.setEnablePin(leftEn);

    stepperRight.setEnablePin(rightEn);

    stepperMid.setMaxSpeed(MAX_SPEED);
    stepperMid.setAcceleration(ACCEL);
}

void AccelStepperController::setup(int windowWidth) {
    // Calculate the correct X: Top left corner is (0,0)
    currentX = botWidth / 2.0;

    // Calculate the right rope length: Assume start top left
    currentRight = windowWidth - botWidth;

    // Stepper setup
    stepperLeft.setCurrentPosition(currentLeft * STEPS_PER_MM);
    stepperRight.setCurrentPosition(rightInversionFactor * currentRight * STEPS_PER_MM);

    stepperMid.setCurrentPosition(0);
    this->windowWidth = windowWidth;
}

void AccelStepperController::enqueueWaypoint(int x, int y) {
    x = std::clamp(x, static_cast<int>(botWidth / 2.0), static_cast<int>(windowWidth - botWidth / 2.0));
    y = std::max(0, y);

    Serial.printf("AccelStepperController.cpp: Enqueued Target: x=%d, y=%d\n", x, y);
    waypoints.emplace(x, y);
}

void AccelStepperController::next() {
    if (waypoints.empty()) return;

    auto target = waypoints.front();

    double x = target.first;
    double y = target.second;

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

    double distance = std::hypot(diffX, diffY);
    if (distance < 0.01) {
        moving = false;
        return;
    }

    double scale = MAX_MM_CHANGE / distance;
    xChange = diffX * scale;
    yChange = diffY * scale;

    moving = true;
}

void AccelStepperController::updateMovement() {
    // If not moving, skip
    if (!moving) return;

    // Find current left/right length
    currentLeft = stepperLeft.currentPosition() / STEPS_PER_MM;
    currentRight = rightInversionFactor * stepperRight.currentPosition() / STEPS_PER_MM;

    // Find current left/right coordinate
    double xLeft = (std::pow(currentLeft, 2) - std::pow(currentRight, 2) + std::pow(windowWidth - botWidth, 2)) / (2 * (windowWidth - botWidth));
    currentX = xLeft + botWidth / 2.0;

    // Clamp to prevent sqrt of negative due to rounding errors
    double ySquared = std::pow(currentLeft, 2) - std::pow(xLeft, 2);
    currentY = (ySquared > 0.0) ? std::sqrt(ySquared) : 0.0;

    // If distanceToGo less than or equal to the recalculate distance, and if the moveTo haven't set to the final x, recalculate
    double distanceToGo = std::hypot(stepperLeft.distanceToGo(), stepperRight.distanceToGo()) / STEPS_PER_MM;
    if (distanceToGo <= RECALC_DIST && (moveToX != targetX || moveToY != targetY)) {
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

        leftMoveTo = std::hypot(leftMoveToX, moveToY);
        rightMoveTo = std::hypot(windowWidth - rightMoveToX, moveToY);

        stepperLeft.moveTo(leftMoveTo * STEPS_PER_MM);
        stepperRight.moveTo(rightInversionFactor * rightMoveTo * STEPS_PER_MM);
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

        // Clamp speed
        leftSpeed = std::min(leftSpeed, static_cast<double>(MAX_SPEED));
        rightSpeed = std::min(rightSpeed, static_cast<double>(MAX_SPEED));

        double leftDir = (leftMoveTo >= lastLeft) ? 1.0 : -1.0;
        double rightDir = (rightMoveTo >= lastRight) ? -1.0 : 1.0;

        stepperLeft.setSpeed(leftSpeed * leftDir);
        stepperRight.setSpeed(rightSpeed * rightDir);
    }

    leftMotorSpeed = (std::abs(stepperLeft.distanceToGo()) > 1) ? stepperLeft.speed() / STEPS_PER_MM : 0;
    rightMotorSpeed = (std::abs(stepperRight.distanceToGo()) > 1) ? stepperRight.speed() / STEPS_PER_MM : 0;

    stepperLeft.runSpeedToPosition();
    stepperRight.runSpeedToPosition();

    // If the moveTo is the target and the stepper is almost there (<= 1), set moving to false
    if (moveToX == targetX && moveToY == targetY &&
        std::abs(stepperLeft.distanceToGo()) <= 1 &&
        std::abs(stepperRight.distanceToGo()) <= 1) {
        waypoints.pop();
        moving = false;
    }
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
