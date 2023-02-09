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
constexpr double stepsPerInch {stepsPerRotation / inchPerRotation};

const double stepsPerSecond {rotationsPerSecond * stepsPerRotation};

constexpr double slowFactor {0.9};
static_assert(0 < slowFactor && slowFactor <= 1);
const double gridLimit {slowFactor / sqrt(2) * stepsPerSecond};

GridSpeed speed{gridLimit, gridLimit};

GridSpeed getTrajectory(Position start, Position end) {
    // Might be abuse of notation unless you consider Position as a vector
    const Position delta {end - start};
    // Scale so that the bigger component moves at max grid speed.
    const double scaleFactor {gridLimit / max(delta.x, delta.y)};
    return GridSpeed{scaleFactor * delta.x, scaleFactor * delta.y};
}


StringSpeed getSpeed() {
    // Expects that the speed variable is updated for current trajectory.
    const Tangential tangential(getLengths());
    const Position position(TruePosition(Radial(tangential)), originOffset);

    const auto rateFactor = position.x * speed.x + position.y * speed.y;

    const StringSpeed stringSpeed {
        rateFactor / tangential.left,
        (rateFactor - width * speed.x) / tangential.right
    };

    return stringSpeed;
}

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

void init(Tangential tangential) {
  for (auto& stepper : steppers) {
    stepper.setPinsInverted(false, false, true);
    stepper.setEnablePin(enableMotorPin);
    stepper.setMaxSpeed(maxStepsPerSecond);
    stepper.setSpeed(0);
  }

  originOffset = Radial(tangential).findOffset();
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

void setOffset(double offset) {
  originOffset = offset;
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

Position getPosition() {
  return Position(getRadial(), originOffset);
}

Radial getRadial() {
  return Radial(getTangential());
}

Tangential getTangential() {
  return Tangential(getLengths());
}

double originOffset {10};  // in inches
}