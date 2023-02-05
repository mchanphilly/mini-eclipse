#include "MotorSystem.h"

#include <AccelStepper.h>
#include <assert.h>

void MotorSystem::enable() {
  steppers[1].enableOutputs();
  delay(100);  // Doesn't impact motor stuttering
}

void MotorSystem::disable() {
  delay(50);
  steppers[0].disableOutputs();
}

void MotorSystem::inputToSteps(long* outSteps, double num1, double num2, Unit unit) {
  switch (unit) {
    case Unit::Inch:
        outSteps[0] = lround(stepsPerInch * num1);
        outSteps[1] = lround(stepsPerInch * num2);
        break;
    case Unit::Step:
        outSteps[0] = (long)num1;
        outSteps[1] = (long)num2;
        break;
  }
}

void MotorSystem::setSpeed(long whatSpeed) {
  for (auto& stepper : steppers) {
    stepper.setSpeed(whatSpeed);
  }
}

MotorSystem::MotorSystem():
  steppers{
    {AccelStepper::DRIVER, stepYPin, dirYPin},
    {AccelStepper::DRIVER, stepXPin, dirXPin}
  }
  {}

void MotorSystem::run() {
  steppers[0].runSpeedToPosition();
  steppers[1].runSpeedToPosition();
  
  if (!steppers[0].distanceToGo() && !steppers[1].distanceToGo()) {
    disable();
  }
}

void MotorSystem::init() {
  for (auto& stepper : steppers) {
    stepper.setPinsInverted(false, false, true);
    stepper.setEnablePin(enableMotorPin);
    stepper.setMaxSpeed(maxStepsPerSecond);
  }

  disable();
}

void MotorSystem::step(double leftNum, double rightNum, Unit unit) {
  enable();

  long outSteps[2];
  inputToSteps(outSteps, leftNum, rightNum, unit);
  lengths[0] += outSteps[0];
  lengths[1] += outSteps[1];

  steppers[0].move(outSteps[0]);
  steppers[0].setSpeed(stepsPerSecond);
  steppers[1].move(-outSteps[1]);
  steppers[1].setSpeed(stepsPerSecond);
}

void MotorSystem::go(double leftNum, double rightNum, Unit unit) {
  // Converting to steps first
  long desiredPosition[2];
  inputToSteps(desiredPosition, leftNum, rightNum, unit);

  // See what we need to do to get there from where we are.
  long leftToGo = desiredPosition[0] - lengths[0];
  long rightToGo = desiredPosition[1] - lengths[1];

  step(leftToGo, rightToGo, Unit::Step);
}

void MotorSystem::zero(double leftLength, double rightLength) {
  inputToSteps(lengths, leftLength, rightLength, Unit::Inch);
}

void MotorSystem::getLengths(double pair[2], Unit unit) {
  switch (unit) {
      case Unit::Inch:
        pair[0] = lengths[0] / stepsPerInch;
        pair[1] = lengths[1] / stepsPerInch;
        break;
      case Unit::Step:
        // TODO I'm sure there's a better way to write this.
        pair[0] = lengths[0];
        pair[1] = lengths[1];
        break;
  }
}