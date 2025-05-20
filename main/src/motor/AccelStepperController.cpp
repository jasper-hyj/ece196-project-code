#ifndef DUAL_STEPPER_CONTROLLER_H
#define DUAL_STEPPER_CONTROLLER_H

#include <AccelStepper.h>

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

	DualStepperController()
		: stepper1(AccelStepper::DRIVER, STEP1_PIN, DIR1_PIN),
		stepper2(AccelStepper::DRIVER, STEP2_PIN, DIR2_PIN) {}

	void begin() {
		// Enable motors
		pinMode(EN1_PIN, OUTPUT);
		pinMode(EN2_PIN, OUTPUT);
		digitalWrite(EN1_PIN, LOW);
		digitalWrite(EN2_PIN, LOW);

		// Stepper setup
		stepper1.setMaxSpeed(SPEED);
		stepper1.setAcceleration(ACCEL);
		stepper1.setCurrentPosition(0);

		stepper2.setMaxSpeed(SPEED);
		stepper2.setAcceleration(ACCEL);
		stepper2.setCurrentPosition(0);
	}

	void moveForwardOneRevolution() {
		stepper1.moveTo(10 * STEPS_PER_REV);
		stepper2.moveTo(10 * STEPS_PER_REV);
		runBoth();
	}

	void moveBackToZero() {
		stepper1.moveTo(0);
		stepper2.moveTo(0);
		runBoth();
	}

	void runBoth() {
		while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0) {
		stepper1.run();
		stepper2.run();
		}
	}

private:
	AccelStepper stepper1;
	AccelStepper stepper2;
	};

#endif // DUAL_STEPPER_CONTROLLER_H
