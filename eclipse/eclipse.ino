#include <Stepper.h>
#include <assert.h>
#include "MotorSystem.h"
#include "Parser.h"
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

MotorSystem motors(numSteps, stepYPin, dirYPin, stepXPin, dirXPin);
Parser parser;

String string;
int stepCount = 0;

void setup() {
  pinMode(enableMotorPin, OUTPUT);
  motors.setSpeed(motorRPM);
  Serial.begin(9600);
  motors.init();
}

void loop() {
  // Verify 
  if(Serial.available()){
      string = Serial.readStringUntil('\n');

      // Serial.println("Received: " + string);
      // if (command.equals("move")) {
      //   motors.step(numSteps, numSteps);
      // }
      auto command = parser.parse(string);
      int num1 = command.num1;
      int num2 = command.num2;

      auto commandType = command.type;
      
      switch (commandType) {
        case Parser::CommandType::Zero:
          stepCount = 0;
          break;

        case Parser::CommandType::GetSteps:
          Serial.println(stepCount);
          break;

        case Parser::CommandType::MoveStep:
          motors.step(num1, num2, StringMotor::Unit::Step);
          stepCount += num1;
          break;

        case Parser::CommandType::MoveInch:
          motors.step(num1, num2, StringMotor::Unit::Inch);
          break;
      }
  }

  // motors.step(numSteps, numSteps);
  // delay(3000);
}
