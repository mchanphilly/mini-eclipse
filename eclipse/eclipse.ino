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
const int motorRPM = 200;
static_assert(motorRPM <= maxMotorRPM);

MotorSystem motors(numSteps, stepYPin, dirYPin, stepXPin, dirXPin);
Parser parser;

String string;

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
      float num1 = command.num1;
      float num2 = command.num2;

      switch (command.type) {
        case Parser::CommandType::Zero:
          motors.zero();  // TODO individual zeroing?
          break;

 

        case Parser::CommandType::MoveStep:
          Serial.println("eueeu");
          motors.step(num1, num2, StringMotor::Unit::Step);
          break;

        case Parser::CommandType::MoveInch:
          motors.step(num1, num2, StringMotor::Unit::Inch);
          break;

        case Parser::CommandType::GetSteps:
          auto steps = motors.getSteps(); 
          Serial.print(steps[0]);
          Serial.print(",");
          Serial.println(steps[1]);
          break;
      }
  }

  // motors.step(numSteps, numSteps);
  // delay(3000);
}
