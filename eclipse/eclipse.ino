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

const auto initialStrings = BlockerSystem::Tangential(8, 42);

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
void zeroFromTangential(BlockerSystem::Tangential tangential) {
  // Blocker zeros on tangentials
  blocker.softZero(tangential);
  auto state = blocker.getStringState();

  auto trueLength = state.toTotalLengths();
  // Motors zero on true lengths.
  motors.zero(trueLength.left, trueLength.right);
}

void go(double pair[2]) {
  const auto newPosition = BlockerSystem::Position(pair);
  blocker.update(newPosition);
  const auto state = blocker.getStringState();
  const auto lengths = state.toTotalLengths();
  motors.go(lengths.left, lengths.right, MotorSystem::Unit::Inch);
}

void execute(Parser::Command command) {
  double num1 = command.num1;
  double num2 = command.num2;

  double pair[2] = {num1, num2};
  double lengths[2];
  auto state = blocker.getStringState();

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
      Serial.println(state.toTangential());
      break;

    case Parser::CommandType::SoftZero:
      zeroFromTangential(BlockerSystem::Tangential(pair));
      break;

    // case Parser::CommandType::HardZero:
    //   blocker.hardZero(pair);
    //   zeroMotorsFromTangentials(pair);
    //   break;

    case Parser::CommandType::Go:
      go(pair);
      break;

    case Parser::CommandType::GetPosition:
      Serial.println(state.toPosition());
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

  zeroFromTangential(initialStrings);
  Serial.println(blocker.getStringState());
}

void loop() {
  // Verify
  if (Serial.available()) {
    string = Serial.readStringUntil('\n');

    auto command = parser.parse(string);
    execute(command);
    Serial.println(blocker.getStringState());
  }

  motors.run();
}
