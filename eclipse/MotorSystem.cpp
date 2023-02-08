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

MotorSystem::Steps MotorSystem::inputToSteps(double num1, double num2, Unit unit) {
  Steps outSteps;
  switch (unit) {
    case Unit::Inch:
        outSteps.left = lround(stepsPerInch * num1);
        outSteps.right = lround(stepsPerInch * num2);
        break;
    case Unit::Step:
        outSteps.left = (long)num1;
        outSteps.right = (long)num2;
        break;
  }
  return outSteps;
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
  steppers[0].setSpeed(stepsPerSecond);
  steppers[1].setSpeed(stepsPerSecond);

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
  auto steps = inputToSteps(leftNum, rightNum, unit);

  enable();
  steppers[0].move(steps.left);
  steppers[1].move(-steps.right);
}

void MotorSystem::go(double leftNum, double rightNum, Unit unit) {
  // Converting to steps first
  auto steps = inputToSteps(leftNum, rightNum, unit);

  enable();
  steppers[0].moveTo(steps.left);
  steppers[1].moveTo(-steps.right);
}

void MotorSystem::zero(double leftLength, double rightLength) {
  auto steps = inputToSteps(leftLength, rightLength, Unit::Inch);
  steppers[0].setCurrentPosition(steps.left);
  steppers[1].setCurrentPosition(-steps.right);
}

void MotorSystem::getLengths(double pair[2], Unit unit) {
  auto steps = getSteps();
  switch (unit) {
      case Unit::Inch:
        pair[0] = steps.left / stepsPerInch;
        pair[1] = steps.right / stepsPerInch;
        break;
      case Unit::Step:
        // TODO I'm sure there's a better way to write this.
        pair[0] = steps.left;
        pair[1] = steps.right;
        break;
  }
}

MotorSystem::Steps MotorSystem::getSteps() {
  Steps steps;
  steps.left = steppers[0].currentPosition();
  steps.right = steppers[1].currentPosition();
  return steps;
}