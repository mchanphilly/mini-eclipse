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

    switch (command.type) {
      case Parser::CommandType::Zero:
        motors.zero();  // TODO individual zeroing?
        break;

      case Parser::CommandType::MoveStep:
        motors.step(num1, num2, MotorSystem::Unit::Step);
        break;

      case Parser::CommandType::GoStep:
        motors.go(num1, num2, MotorSystem::Unit::Step);
        break;

      case Parser::CommandType::MoveInch:
        motors.step(num1, num2, MotorSystem::Unit::Inch);
        break;

      case Parser::CommandType::GoInch:
        motors.go(num1, num2, MotorSystem::Unit::Inch);

      case Parser::CommandType::GetSteps:
        auto steps = motors.getSteps(); 
        Serial.print(steps[0]);
        Serial.print(",");
        Serial.println(steps[1]);
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
  if(Serial.available()){
      string = Serial.readStringUntil('\n');

      auto command = parser.parse(string);
      execute(command);
  }

  // motors.step(numSteps, numSteps);
  // delay(3000);
}
