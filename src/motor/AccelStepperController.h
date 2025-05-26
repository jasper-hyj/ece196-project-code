#ifndef ACCEL_STEPPER_CONTROLLER_H
#define ACCEL_STEPPER_CONTROLLER_H

#include <AccelStepper.h>
#include <ArduinoJson.h>

#include <queue>
#include <sstream>
#include <string>

class AccelStepperController {
   public:
    // Constants
    static constexpr int STEPS_PER_REV = 1600;  // 1/8 microstepping
    static constexpr int MM_PER_REV = 20;
    static constexpr int STEPS_PER_MM = STEPS_PER_REV / MM_PER_REV;

    static constexpr int MAX_SPEED = 2000;  // Steps per sec
    static constexpr int ACCEL = 1000;

    static constexpr int MAX_MM_CHANGE = 10;

    AccelStepperController(
        int leftEn, int leftStep, int leftDir,
        int rightEn, int rightStep, int rightDir,
        double windowWidth, double botWidth);

    void begin(int windowWidth);

    void enqueueTarget(double x, double y);

    void next();
    void updateMovement();
    void stop();

    bool isMoving() const { return moving; }

    double getCurrentX() const { return currentX; }
    double getCurrentY() const { return currentY; }
    double getTargetX() const { return targetX; }
    double getTargetY() const { return targetY; }

    mutable std::string result;

    const char* toString() {
        std::ostringstream oss;
        oss << "AccelStepper("
            << "moving: " << (moving ? "true" : "false") << ", "
            << "currentX: " << currentX << ", "
            << "currentY: " << currentY << ", "
            << "targetX: " << targetX << ", "
            << "targetY: " << targetY << ", "
            << "currentLeft: " << currentLeft << ", "
            << "currentRight: " << currentRight << ", "
            << "leftMoveTo: " << leftMoveTo << ", "
            << "rightMoveTo: " << rightMoveTo << ")";
        result = oss.str();
        return result.c_str();
    }
    JsonDocument toJSON() const {
        JsonDocument doc;

        doc["timestamp"] = millis();

        doc["moving"] = moving;
        doc["currentX"] = currentX;
        doc["currentY"] = currentY;
        doc["targetX"] = targetX;
        doc["targetY"] = targetY;
        doc["moveToX"] = moveToX;
        doc["moveToY"] = moveToY;
        doc["xChange"] = xChange;
        doc["yChange"] = yChange;

        doc["currentLeft"] = currentLeft;
        doc["currentRight"] = currentRight;
        doc["leftMoveTo"] = leftMoveTo;
        doc["rightMoveTo"] = rightMoveTo;

        doc["leftMotorSpeed"] = leftMotorSpeed;
        doc["rightMotorSpeed"] = rightMotorSpeed;

        return doc;
    }

   private:
    static AccelStepperController* instance;
    const int leftEn;
    const int rightEn;
    const double botWidth;

    AccelStepper stepperLeft;
    AccelStepper stepperRight;

    double windowWidth;

    std::queue<std::pair<int, int>> targets;

    bool moving = false;

    double currentX = 0, currentY = 0;  // Current position of bot, Unit: (mm)
    double moveToX = 0, moveToY = 0;    // Move to position of bot, Unit: (mm)
    double targetX = 0, targetY = 0;    // Target position of bot, Unit: (mm)
    double xChange = 0, yChange = 0;    // Change per loop of bot, Unit: (mm)

    double lastLeft = 0, lastRight = 0;        // Last
    double currentLeft = 0, currentRight = 0;  // Rope length, Unit: (mm)
    double leftMoveTo = 0, rightMoveTo = 0;

    double leftMotorSpeed = 0, rightMotorSpeed = 0;
};

#endif