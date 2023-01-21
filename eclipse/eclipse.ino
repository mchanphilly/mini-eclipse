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

const int numSteps = 800;

MotorSystem motors(numSteps, stepYPin, dirYPin, stepXPin, dirXPin);
BlockerSystem blocker;
Parser parser;

String string;

void zero(double pair[2]) {
  motors.getLengths(pair, MotorSystem::Unit::Inch);
  blocker.zero(pair);
}

void printPair(double pair[2]) {
  Serial.print(pair[0]);
  Serial.print(",");
  Serial.println(pair[1]);
}

void execute(Parser::Command command) {
  double num1 = command.num1;
  double num2 = command.num2;

  double pair[2] = {num1, num2};
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
      motors.getLengths(pair, unit);
      printPair(pair);
      break;

    case Parser::CommandType::Zero:
        // assert(num1 > 0);
        // assert(num2 > 0);
        motors.zero(num1, num2);
        zero(pair);
        break;

    case Parser::CommandType::Go:
        blocker.getLengthsFromPosition(pair, (double)num1, (double)num2);
        motors.go(pair[0], pair[1], MotorSystem::Unit::Inch);
        printPair(pair);
        break;

    case Parser::CommandType::GetPosition:
        blocker.getPosition(pair);
        printPair(pair);
        break;


    default:
      Serial.println("Bad command");
      break;
  }
}

void setup() {
  pinMode(enableMotorPin, OUTPUT);
  Serial.begin(9600);
  motors.init();

  double pair[2];
  zero(pair);

  printPair(pair);
}

void loop() {
  // Verify
  if (Serial.available()) {
    string = Serial.readStringUntil('\n');

    auto command = parser.parse(string);
    execute(command);
  }
}
