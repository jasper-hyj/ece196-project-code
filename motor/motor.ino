// Include the Arduino Stepper Library
#include <Stepper.h>

#define STEPS 200
#define motorInterfaceType 1

// first motor
const unsigned int in1 = 36;
const unsigned int in2 = 48;
const unsigned int EnA = 35;

// second motor
const unsigned int in3 = 34;
const unsigned int in4 = 47;
const unsigned int EnB = 33;

// third motor
const unsigned int in5 = 12;
const unsigned int in6 = 10;
const unsigned int EnC = 11;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;


// Create Instance of Stepper library
Stepper motor1(STEPS, in1, in2, in3, in4);
Stepper motor2(STEPS, in5, in6);


//out1 - pin 1
//out2 - pin 3 
//out3 - pin 4
//out4 - pin 2

void setup()
{
	// set the speed at 60 rpm:
	motor1.setSpeed(60);
    motor2.setSpeed(60);
	// initialize the serial port:
	Serial.begin(9600);
    analogWrite(EnA, 255);
    analogWrite(EnB, 255);
    // analogWrite(EnC, 255);
    
}

void loop() {
	// step one revolution in one direction:
	Serial.println("clockwise");
	motor1.step(1000);
	delay(500);
}