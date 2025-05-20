#ifndef ACCEL_STEPPER_CONTROLLER_H
#define ACCEL_STEPPER_CONTROLLER_H

class AccelStepperController {
public:
    // Motor pin definitions
    static constexpr int EN1_PIN   = 1;
    static constexpr int STEP1_PIN = 2;
    static constexpr int DIR1_PIN  = 3;

    static constexpr int EN2_PIN   = 4;
    static constexpr int STEP2_PIN = 5;
    static constexpr int DIR2_PIN  = 6;

    // Constants
    static constexpr long STEPS_PER_REV = 1600; // 1/8 microstepping
    static constexpr int SPEED = 1500;
    static constexpr int ACCEL = 1000;

    AccelStepperController();

private:
    AccelStepper stepperLeft;
    AccelStepper stepperRight;
}