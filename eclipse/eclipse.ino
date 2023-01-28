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

const double initialTangential[2] = {8, 42};

MotorSystem motors(stepYPin, dirYPin, stepXPin, dirXPin);
BlockerSystem blocker;
Parser parser;

String string;

void printPair(double pair[2]) {
  Serial.print(pair[0]);
  Serial.print(",");
  Serial.println(pair[1]);
}

// Input tangentials
void zeroMotorsFromTangentials(const double tangential[2]) {
  // Blocker zeros on tangentials
  double lengths[2];
  blocker.getLengthsFromTangential(lengths, tangential);

  // Motors zero on true lengths.
  motors.zero(lengths[0], lengths[1]);
  printPair(lengths);
}

void execute(Parser::Command command) {
  double num1 = command.num1;
  double num2 = command.num2;

  double pair[2] = {num1, num2};
  double lengths[2];
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
      // Note that this includes the arc.
      motors.getLengths(lengths, unit);
      printPair(lengths);
      break;

    case Parser::CommandType::GetInch:
      // Note that this does NOT include the arc
      blocker.getTangential(lengths);
      printPair(lengths);
      break;

    case Parser::CommandType::SoftZero:
        blocker.softZero(pair);
        zeroMotorsFromTangentials(pair);
        break;

    case Parser::CommandType::HardZero:
        blocker.hardZero(pair);
        zeroMotorsFromTangentials(pair);
        break;

    case Parser::CommandType::Go:
        blocker.getLengthsFromPosition(lengths, pair);
        motors.go(lengths[0], lengths[1], MotorSystem::Unit::Inch);
        
        blocker.setPosition(pair);
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

  blocker.hardZero(initialTangential);
  zeroMotorsFromTangentials(initialTangential);
}

void loop() {
  // Verify
  if (Serial.available()) {
    string = Serial.readStringUntil('\n');

    auto command = parser.parse(string);
    execute(command);
  }

  motors.run();
}
