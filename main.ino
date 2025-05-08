#include <Wire.h>
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

// first ultra sonic sensor
const unsigned int trig1 = 26;
const unsigned int echo1 = 21;

// second ultra sonic sensor
const unsigned int trig2 = 37;
const unsigned int echo2 = 38;

// used for calculating distance for ultra sonic sensor
long duration;
float distanceCm;


void setup(){
    Serial.begin(115200);

    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(EnA, OUTPUT);

    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(EnB, OUTPUT);

    pinMode(in5, OUTPUT);
    pinMode(in6, OUTPUT);
    pinMode(EnC, OUTPUT);

    pinMode(trig1, OUTPUT);
    pinMode(echo1, INPUT);

    pinMode(trig2, OUTPUT);
    pinMode(echo2, INPUT);
}

void loop(){


}