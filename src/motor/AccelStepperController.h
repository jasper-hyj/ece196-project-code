#ifndef ACCEL_STEPPER_CONTROLLER_H
#define ACCEL_STEPPER_CONTROLLER_H

#include <AccelStepper.h>

#include "../Direction.h"

class AccelStepperController {
public:
    // Constants
    static constexpr int STEPS_PER_REV = 1600; // 1/8 microstepping
	static constexpr int MAX_SPEED = 1500;
    static constexpr int ACCEL = 1000;
    static constexpr int MM_PER_REV = 20;

    AccelStepperController(
        int leftEn, int leftStep, int leftDir,
        int rightEn, int rightStep, int rightDir,
        double windowsWidth, double botWidth
    );

    void begin();

    double getCurrentX();

    double getCurrentY();

    double getCurrentLeftLength();

    double getCurrentRightLength();
    
    void move(Direction direction);

    void moveToPosition(double x, double y);
    void stop();

private:
    static AccelStepperController* instance;
    int leftEn;
    int rightEn;

    AccelStepper stepperLeft;
    AccelStepper stepperRight;

	double windowsWidth;
	double botWidth;

    double leftLength = 0;
    double rightLength = 0;

    double currentX = 0;
    double currentY = 0;

    void runBoth();
};

#endif