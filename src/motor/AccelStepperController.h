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
    static constexpr int STEPS_PER_REV = 200;
    static constexpr int MM_PER_REV = 20;
    static constexpr int STEPS_PER_MM = STEPS_PER_REV / MM_PER_REV;

    static constexpr int MAX_SPEED = 1000;  // Unit: (step/s)
    static constexpr int ACCEL = 1000;

    static constexpr int MAX_MM_CHANGE = 10;

    AccelStepperController(
        int leftEn, int leftStep, int leftDir,
        int rightEn, int rightStep, int rightDir,
        double botWidth);

    void begin(int windowWidth);

    void enqueueWaypoint(double x, double y);

    void next();
    void updateMovement();
    void stop();

    bool isMoving() const { return moving; }

    double getCurrentX() const { return currentX; }
    double getCurrentY() const { return currentY; }
    double getTargetX() const { return targetX; }
    double getTargetY() const { return targetY; }

    mutable std::string result;

    void toJSON(JsonDocument& jsonDoc) const {

        
        JsonObject accelStepperJson = jsonDoc["accelStepper"].to<JsonObject>();
        
        accelStepperJson["timestamp"] = millis();
        accelStepperJson["moving"] = moving;
        accelStepperJson["currentX"] = currentX;
        accelStepperJson["currentY"] = currentY;
        accelStepperJson["targetX"] = targetX;
        accelStepperJson["targetY"] = targetY;
        accelStepperJson["moveToX"] = moveToX;
        accelStepperJson["moveToY"] = moveToY;
        accelStepperJson["xChange"] = xChange;
        accelStepperJson["yChange"] = yChange;

        accelStepperJson["currentLeft"] = currentLeft;
        accelStepperJson["currentRight"] = currentRight;
        accelStepperJson["leftMoveTo"] = leftMoveTo;
        accelStepperJson["rightMoveTo"] = rightMoveTo;

        accelStepperJson["leftMotorSpeed"] = leftMotorSpeed;
        accelStepperJson["rightMotorSpeed"] = rightMotorSpeed;

        JsonArray wpArray = accelStepperJson["waypoints"].to<JsonArray>();
        std::queue<std::pair<int, int>> temp = waypoints;
        while (!temp.empty()) {
            const auto& front = temp.front();
            int x = front.first;
            int y = front.second;
            JsonObject point = wpArray.add<JsonObject>();
            point["x"] = x;
            point["y"] = y;
            temp.pop();
        }
    }

   private:
    static AccelStepperController* instance;
    const int leftEn;
    const int rightEn;
    const double botWidth;

    AccelStepper stepperLeft;
    AccelStepper stepperRight;

    double windowWidth;

    std::queue<std::pair<int, int>> waypoints;

    bool moving = false;

    double currentX = 0, currentY = 0;  // Current position of bot, Unit: (mm)
    double moveToX = 0, moveToY = 0;    // Move to position of bot, Unit: (mm)
    double targetX = 0, targetY = 0;    // Target position of bot, Unit: (mm)
    double xChange = 0, yChange = 0;    // Change per loop of bot, Unit: (mm)

    double lastLeft = 0, lastRight = 0;         // Last
    double currentLeft = 0, currentRight = 0;   // Rope length, Unit: (mm)
    double leftMoveTo = 0, rightMoveTo = 0;

    double leftMotorSpeed = 0, rightMotorSpeed = 0; // Speed: Unit (step/s)
};

#endif