#ifndef ACCEL_STEPPER_CONTROLLER_H
#define ACCEL_STEPPER_CONTROLLER_H

#include <AccelStepper.h>

#include "../Direction.h"

class AccelStepperController {
public:
    // Constants
	static constexpr int MAX_SPEED = 1500;
    static constexpr int ACCEL = 1000;

    static constexpr int

    AccelStepperController(
        int leftEn, int leftStep, int leftDir,
        int rightEn, int rightStep, int rightDir);

    void begin();
    
    void move(Direction direction);

    void moveToPosition(double x, double y);
    void stop();

private:
    int leftEn;
    int rightEn;

    AccelStepper stepperLeft;
    AccelStepper stepperRight;

	int MM_PER_STEP;

	double windowsWidth;
	double botWidth;

    double leftLength = 0;
    double rightLength = 0;

    double currentX = 0;
    double currentY = 0;

    void runBoth();
}