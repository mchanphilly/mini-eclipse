#include <Stepper.h>
#include <assert.h>
#include "StringMotorPair.h"
// Mini-Eclipse project (January 2023)
// Martin Chan (philadelphia@mit.edu)


const int stepXPin = 2;
const int dirXPin = 5;
const int stepYPin = 3;
const int dirYPin = 6;
const int enableMotorPin = 8;

// TODO verify that numSteps is really 800 per revolution.
const int numSteps = 800;

// motorRPM 345 max
const int maxMotorRPM = 345;
const int motorRPM = 300;
static_assert(motorRPM <= maxMotorRPM);

StringMotorPair motors(numSteps, stepYPin, dirYPin, stepXPin, dirXPin);
String command;

void setup() {
  motors.setSpeed(motorRPM);
  pinMode(enableMotorPin, OUTPUT);
  Serial.begin(9600);
  motors.init();
}

void loop() {
  // Verify 
  if(Serial.available()){
      command = Serial.readStringUntil('\n');

      Serial.println("Received: " + command);
      if (command.equals("move")) {
        motors.step(numSteps, numSteps);
      }

  }

  // motors.step(numSteps, numSteps);
  // delay(3000);
}
