#include <Stepper.h>
#include <assert.h>
#include "BlockerSystem.h"
#include "MotorSystem.h"
#include "Parser.h"
// Mini-Eclipse project (January 2023)
// Martin Chan (philadelphia@mit.edu)


const int stepXPin = 2;
const int dirXPin = 5;
const int stepYPin = 3;
const int dirYPin = 6;
const int enableMotorPin = 8;

// motorRPM 345 max
const int maxMotorRPM = 345;
const int motorRPM = 220;
static_assert(motorRPM <= maxMotorRPM);

const int numSteps = 800;


BlockerSystem blocker();
MotorSystem motors(numSteps, stepYPin, dirYPin, stepXPin, dirXPin);
Parser parser;

String string;

void execute(Parser::Command command) {
  float num1 = command.num1;
  float num2 = command.num2;

  // If only this was C++17 and not C++11
  // using enum Parser::CommandType;

  MotorSystem::Unit unit = MotorSystem::Unit::Invalid;
  switch (command.type) {
    case Parser::CommandType::ShiftStep:
    case Parser::CommandType::GoStep:
    case Parser::CommandType::GetStep:
      unit = MotorSystem::Unit::Step;
      break;

    case Parser::CommandType::ShiftInch:
    case Parser::CommandType::GoInch:
    case Parser::CommandType::GetInch:
      unit = MotorSystem::Unit::Inch;
      break;
  }

  switch (command.type) {
    case Parser::CommandType::Zero:
      motors.zero();  // TODO individual zeroing?
      break;

    case Parser::CommandType::ShiftStep:
    case Parser::CommandType::ShiftInch:
      motors.step(num1, num2, unit);
      break;

    case Parser::CommandType::GoStep:
    case Parser::CommandType::GoInch:
      motors.go(num1, num2, unit);
      break;

    case Parser::CommandType::GetStep:
    case Parser::CommandType::GetInch:
      float pair[2];
      
      motors.getStep(pair, unit);
      Serial.print(pair[0]);
      Serial.print(",");
      Serial.println(pair[1]);
      break;
  }
}

void setup() {
  pinMode(enableMotorPin, OUTPUT);
  motors.setSpeed(motorRPM);
  Serial.begin(9600);
  motors.init();
}

void loop() {
  // Verify
  if (Serial.available()) {
    string = Serial.readStringUntil('\n');

    auto command = parser.parse(string);
    execute(command);
  }

  // motors.step(numSteps, numSteps);
  // delay(3000);
}
