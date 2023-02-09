#include "MotorSystem.h"

#include <AccelStepper.h>
#include "Lengths.h"

namespace MotorSystem {
using namespace Lengths;
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
StringSpeed lastSpeed;

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

Steps inchToSteps(TotalLengths lengths) {
  Steps outSteps;
  outSteps.left = lround(stepsPerInch * lengths.left);
  outSteps.right = lround(stepsPerInch * lengths.right);
  return outSteps;
}

}

const double stepsPerSecond {rotationsPerSecond * stepsPerRotation};
const double inchRadius {stepRadius / stepsPerInch};

void run(StringSpeed speed) {
  lastSpeed = speed;
  steppers[0].setSpeed(speed.left);
  steppers[1].setSpeed(speed.right);

  steppers[0].runSpeedToPosition();
  steppers[1].runSpeedToPosition();
  
  if (!steppers[0].distanceToGo() && !steppers[1].distanceToGo()) {
    disable();
  }
}

void run() {
    run(lastSpeed);
}

void init() {
  for (auto& stepper : steppers) {
    stepper.setPinsInverted(false, false, true);
    stepper.setEnablePin(enableMotorPin);
    stepper.setMaxSpeed(maxStepsPerSecond);
    stepper.setSpeed(0);
  }

  disable();
}

void step(TotalLengths lengths) {
  auto steps = inchToSteps(lengths);
  step(steps);
}

void step(Steps steps) {
  enable();
  steppers[0].move(steps.left);
  steppers[1].move(-steps.right);
}

void go(TotalLengths lengths) {
  // Converting to steps first
  auto steps = inchToSteps(lengths);
  go(steps);
}

void go(Steps steps) {
  enable();
  steppers[0].moveTo(steps.left);
  steppers[1].moveTo(-steps.right);
}

void zero(TotalLengths lengths) {
  auto steps = inchToSteps(lengths);
  steppers[0].setCurrentPosition(steps.left);
  steppers[1].setCurrentPosition(-steps.right);
}

TotalLengths getLengths() {
  TotalLengths lengths;
  auto steps = getSteps();
  lengths.left = static_cast<double>(steps.left) / stepsPerInch;
  lengths.right = static_cast<double>(steps.right) / stepsPerInch;  // Note negative
  return lengths;
}

Steps getSteps() {
  Steps steps;
  steps.left = steppers[0].currentPosition();
  steps.right = -steppers[1].currentPosition();
  return steps;
}

}