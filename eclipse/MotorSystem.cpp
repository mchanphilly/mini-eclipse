#include "MotorSystem.h"

#include <AccelStepper.h>
#include <assert.h>

namespace MotorSystem {
namespace {
constexpr int stepXPin {2};
constexpr int dirXPin {5};
constexpr int stepYPin {3};
constexpr int dirYPin {6};

// Left and right
AccelStepper steppers[2] {
    {AccelStepper::DRIVER, stepYPin, dirYPin},
    {AccelStepper::DRIVER, stepXPin, dirXPin}
};

constexpr int enableMotorPin {8};

constexpr int stepsInMotor {200};
constexpr int microsteps {8};
constexpr double stepsPerRotation {stepsInMotor * microsteps};

constexpr int secondsPerMinute {60};

constexpr double maxRotationsPerMinute {500};
constexpr double maxRotationsPerSecond {maxRotationsPerMinute / secondsPerMinute};
constexpr double maxStepsPerSecond {maxRotationsPerSecond * stepsPerRotation};

constexpr double rotationsPerMinute {320};
static_assert(rotationsPerMinute <= maxRotationsPerMinute);
constexpr double rotationsPerSecond {rotationsPerMinute / secondsPerMinute};


// Parameters of the stepper motors.

constexpr double stepRadius = {stepsPerRotation / (2*PI)};

// // Parameters of the string set-up (including initial conditions)
constexpr double inchPerRotation {2.37578};
constexpr double stepsPerInch {stepsPerRotation / inchPerRotation};

void enable() {
  steppers[1].enableOutputs();
  delay(100);  // Doesn't impact motor stuttering
}

void disable() {
  delay(50);
  steppers[0].disableOutputs();
}

Steps inputToSteps(double num1, double num2, Unit unit) {
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

void setSpeed(long whatSpeed) {
  for (auto& stepper : steppers) {
    stepper.setSpeed(whatSpeed);
  }
}
}

const double stepsPerSecond {rotationsPerSecond * stepsPerRotation};
const double inchRadius {stepRadius / stepsPerInch};

void run() {
  steppers[0].setSpeed(stepsPerSecond);
  steppers[1].setSpeed(stepsPerSecond);

  steppers[0].runSpeedToPosition();
  steppers[1].runSpeedToPosition();
  
  if (!steppers[0].distanceToGo() && !steppers[1].distanceToGo()) {
    disable();
  }
}

void init() {
  for (auto& stepper : steppers) {
    stepper.setPinsInverted(false, false, true);
    stepper.setEnablePin(enableMotorPin);
    stepper.setMaxSpeed(maxStepsPerSecond);
  }

  disable();
}

void step(double leftNum, double rightNum, Unit unit) {
  auto steps = inputToSteps(leftNum, rightNum, unit);

  enable();
  steppers[0].move(steps.left);
  steppers[1].move(-steps.right);
}

void go(double leftNum, double rightNum, Unit unit) {
  // Converting to steps first
  auto steps = inputToSteps(leftNum, rightNum, unit);

  enable();
  steppers[0].moveTo(steps.left);
  steppers[1].moveTo(-steps.right);
}

void zero(double leftLength, double rightLength) {
  auto steps = inputToSteps(leftLength, rightLength, Unit::Inch);
  steppers[0].setCurrentPosition(steps.left);
  steppers[1].setCurrentPosition(-steps.right);
}

void getLengths(double pair[2], Unit unit) {
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

Steps getSteps() {
  Steps steps;
  steps.left = steppers[0].currentPosition();
  steps.right = steppers[1].currentPosition();
  return steps;
}

}