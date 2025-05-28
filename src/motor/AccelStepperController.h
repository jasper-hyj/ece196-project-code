#ifndef ACCEL_STEPPER_CONTROLLER_H
#define ACCEL_STEPPER_CONTROLLER_H
#define M_PI 3.14159265358979323846
#define R_SENSE 0.11f  // Sense resistor value for TMC2209
#define DRIVER_ADDRESS 0b00

#include <AccelStepper.h>
#include <ArduinoJson.h>
#include <TMCStepper.h>

#include <queue>
#include <sstream>
#include <string>

class AccelStepperController {
   public:
    // Constants
    static constexpr int STEPS_PER_REV = 3200;                       // Unit: (steps/rev)
    static constexpr int MM_PER_REV = M_PI * 20.0;                   // Unit: (mm/rev)
    static constexpr int STEPS_PER_MM = STEPS_PER_REV / MM_PER_REV;  // Unit: (steps/mm)

    static constexpr int MAX_SPEED = 1000;    // Unit: (steps/s)
    static constexpr int ACCEL = 1000;        // Max acceleration, Unit: (steps/s²)
    static constexpr int MAX_MM_CHANGE = 15;  // Max displacement allow, Unit: (mm)

    static constexpr int RECALC_DIST = 14;  // Recalculate distance(mm)

    AccelStepperController(
        int leftEn, int leftStep, int leftDir, int leftUartRx, int leftUartTx,
        int rightEn, int rightStep, int rightDir, int rightUartRx, int rightUartTx,
        int midIn1, int midIn2, int midIn3, int midIn4, int midEnA, int midEnB,
        double botWidth);

    void begin();

    void setup(int windowWidth);

    void enqueueWaypoint(int x, int y);

    void next();
    void updateMovement();
    void spinMid();
    void stop();

    bool isMoving() const { return moving; }

    mutable std::string result;

    void toJSON(JsonDocument& jsonDoc) const {
        JsonObject accelStepperJson = jsonDoc["accelStepper"].to<JsonObject>();

        accelStepperJson["timestamp"] = millis();
        accelStepperJson["moving"] = moving;

        accelStepperJson["STEPS_PER_REV"] = STEPS_PER_REV;
        accelStepperJson["MM_PER_REV"] = MM_PER_REV;
        accelStepperJson["STEPS_PER_MM"] = STEPS_PER_MM;
        accelStepperJson["MAX_SPEED"] = MAX_SPEED;

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
        accelStepperJson["currentMid"] = currentMid;
        accelStepperJson["leftMoveTo"] = leftMoveTo;
        accelStepperJson["rightMoveTo"] = rightMoveTo;

        accelStepperJson["leftMotorSpeed"] = leftMotorSpeed;
        accelStepperJson["rightMotorSpeed"] = rightMotorSpeed;

        accelStepperJson["midMotorSpeed"] = midMotorSpeed;

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

    const int leftEn, leftUartRx, leftUartTx;
    const int rightEn, rightUartRx, rightUartTx;
    const int midEnA, midEnB;

    HardwareSerial SerialMotorLeft;
    HardwareSerial SerialMotorRight;

    TMC2209Stepper driverLeft;
    TMC2209Stepper driverRight;

    AccelStepper stepperLeft;
    AccelStepper stepperRight;
    AccelStepper stepperMid;

    const double botWidth;
    double windowWidth;

    std::queue<std::pair<int, int>> waypoints;

    bool moving = false;

    const int rightInversionFactor = -1;

    double currentX = 0, currentY = 0;  // Current position of bot, Unit: (mm)
    double moveToX = 0, moveToY = 0;    // Move to position of bot, Unit: (mm)
    double targetX = 0, targetY = 0;    // Target position of bot, Unit: (mm)
    double xChange = 0, yChange = 0;    // Change per loop of bot, Unit: (mm)

    double lastLeft = 0, lastRight = 0;        // Last
    double currentLeft = 0, currentRight = 0;  // Rope length, Unit: (mm)
    double leftMoveTo = 0, rightMoveTo = 0;

    double leftMotorSpeed = 0, rightMotorSpeed = 0;  // Speed: Unit (step/s)

    double currentMid = 0, midMotorSpeed = 0;
};

#endif