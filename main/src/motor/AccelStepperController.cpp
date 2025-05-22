#include <AccelStepperController.h>
#include <cmath>

AccelStepperController* AccelStepperController::instance = nullptr;

AccelStepperController::AccelStepperController(
    int leftEn, int leftStep, int leftDir,
    int rightEn, int rightStep, int rightDir)
    : stepperLeft(AccelStepper::DRIVER, leftStep, leftDir),
      stepperRight(AccelStepper::DRIVER, rightStep, rightDir),
      leftEn(leftEn),
      rightEn(rightEn) {
    instance = this;
}

void AccelStepperController::begin() {
    // Enable motors
    pinMode(leftEn, OUTPUT);
    pinMode(rightEn, OUTPUT);

    digitalWrite(leftEn, 0);
    digitalWrite(rightEn, 0);

    // Stepper setup
    stepperLeft.setMaxSpeed(MAX_SPEED);
    stepperLeft.setAcceleration(ACCEL);
    stepperLeft.setCurrentPosition(0);

    stepperRight.setMaxSpeed(MAX_SPEED);
    stepperRight.setAcceleration(ACCEL);
    stepperRight.setCurrentPosition(0);
}

void AccelStepperController::move(Direction direction) {
    
}

void AccelStepperController::moveToPosition(double x, double y) {

    if (x < 0 || x > windowsWidth) {
        x = currentX;
    }

    if (y < 0) {
        y = currentY;
    }

    // Calculate X location
    double leftX = x - (botwidth / 2.0);
    double rightX = x + (botwidth / 2.0);
    
    // Calculate left/right length
    double targetLeftLength = std::sqrt(std::pow(leftX, 2) + std::pow(y, 2));
    double targetRightLength = std::sqrt(std::pow(windowsWidth - rightX, 2) + std::pow(y, 2));

    // Find the length difference
    double leftSteps = (targetLeftLength - leftLength) / MM_PER_STEP;
    double rightSteps = (targetRightLength - rightLength) / MM_PER_STEP;

    stepperLeft.move(leftSteps);
    stepperRight.move(rightSteps);
    runBoth();

    currentX = x;
    currentY = y;
}

void AccelStepperController::runBoth() {
    while (stepperLeft.distanceToGo() != 0 || stepperRight.distanceToGo() != 0) {
        stepperLeft.run();
        stepperRight.run();
    }
}

