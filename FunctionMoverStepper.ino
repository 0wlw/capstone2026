#include <AccelStepper.h>

// Define the interface type as 'DRIVER' for A4988 (2-pin STEP/DIR control)
#define MOTOR_INTERFACE_TYPE AccelStepper::DRIVER

// Define hardware pin connections
const int stepPin = 6;
const int dirPin = 7;

// Create an instance of the library
AccelStepper stepper(MOTOR_INTERFACE_TYPE, stepPin, dirPin);

void setup() {
  // Set the maximum speed in steps per second
  stepper.setMaxSpeed(1000); 
  
  // Set acceleration in steps per second squared
  stepper.setAcceleration(500); 
  
  // Set target position (200 steps is 1 full rotation for most NEMA 17 motors)
  stepper.setSpeed(18.75); // 250 for spinning plate, 18.75 for function mover
}

void loop() { 
  stepper.runSpeed(); 
}
