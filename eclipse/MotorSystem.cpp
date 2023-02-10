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

constexpr int enableMotorPin {8};

constexpr int stepsInMotor {200};
constexpr int microsteps {8};
constexpr double stepsPerRotation {stepsInMotor * microsteps};

constexpr int secondsPerMinute {60};

constexpr double maxRotationsPerMinute {500};
constexpr double maxRotationsPerSecond {maxRotationsPerMinute / secondsPerMinute};
constexpr double maxStepsPerSecond {maxRotationsPerSecond * stepsPerRotation};

constexpr double rotationsPerMinute {80};
static_assert(rotationsPerMinute <= maxRotationsPerMinute);
constexpr double rotationsPerSecond {rotationsPerMinute / secondsPerMinute};


// Parameters of the stepper motors.

constexpr double stepRadius = {stepsPerRotation / (2*PI)};

// // Parameters of the string set-up (including initial conditions)
constexpr double stepsPerInch {stepsPerRotation / inchPerRotation};

constexpr double safetyFactor = 1.2;
constexpr double safeSteps = stepsPerInch * width * safetyFactor;

const double stepsPerSecond {rotationsPerSecond * stepsPerRotation};

constexpr double slowFactor {0.9};
static_assert(0 < slowFactor && slowFactor <= 1);
const double gridLimit {slowFactor / sqrt(2) * stepsPerSecond};

const StringSpeed defaultStringSpeed {stepsPerSecond, stepsPerSecond};
StringSpeed currentSpeed {defaultStringSpeed};

GridSpeed bearing {gridLimit, gridLimit};

void enable() {
  steppers[1].enableOutputs();
  delay(100);  // Doesn't impact motor stuttering
}

void disable() {
  delay(50);
  steppers[0].disableOutputs();
  currentSpeed = defaultStringSpeed;
}

Steps inchToSteps(TotalLengths lengths) {
  Steps outSteps;
  outSteps.left = lround(stepsPerInch * lengths.left);
  outSteps.right = lround(stepsPerInch * lengths.right);
  return outSteps;
}

GridSpeed normalizeSpeed(TruePosition delta) {
  const auto greater = static_cast<double>(max(delta.x, delta.y));
  const auto getComponent = [greater](double num) {
    return (static_cast<double>(num) / greater) * gridLimit;
  };
  
  const auto out = GridSpeed(getComponent(delta.x), getComponent(delta.y));
  return out;
}

StringSpeed normalizeSpeed(Steps steps) {
  const auto greater = static_cast<long double>(max(steps.left, steps.right));
  const auto getComponent = [greater](long num) {
    return static_cast<double>((static_cast<long double>(num) / greater) * stepsPerSecond);
  };
  return StringSpeed(getComponent(steps.left), getComponent(steps.right));
}

StringSpeed speedFromBearing() {
  const auto truePosition = getTruePosition();
  const auto factor = truePosition.x * bearing.x + truePosition.y * bearing.y;
  const auto tangential = getTangential();
  const auto out = StringSpeed(factor/tangential.left, -(factor - width*bearing.x)/tangential.right);
  Serial.print("Speed from bearing: ");
  Serial.println(out);
  return out;
}

}

bool stillMoving() {
  bool active = steppers[0].distanceToGo() || steppers[1].distanceToGo();
  return active;
}

void run() {
  steppers[0].setSpeed(currentSpeed.left);
  steppers[1].setSpeed(currentSpeed.right);

  steppers[0].runSpeedToPosition();
  steppers[1].runSpeedToPosition();
  if (!stillMoving()) {
    disable();
  }
}

void init(Tangential tangential) {
  for (auto& stepper : steppers) {
    stepper.setPinsInverted(false, false, true);
    stepper.setEnablePin(enableMotorPin);
    stepper.setMaxSpeed(maxStepsPerSecond);
    stepper.setSpeed(0);
  }

  const auto radial = Radial(tangential);
  originOffset = radial.findOffset();
  const auto truePosition = TruePosition(radial);
  const TotalLengths lengths = TotalLengths(truePosition, radial, tangential);
  zero(lengths);
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
  currentSpeed = normalizeSpeed(steps);
}

void go(TotalLengths lengths) {
  auto steps = inchToSteps(lengths);
  if (steps.left + steps.right < safeSteps) {
    Serial.println("Step failed: too close to danger length");
  } else {
    go(steps);
  }
}

void go(Steps steps) {
  enable();
  const auto current = getSteps();
  const auto newSteps = steps - current;
  step(newSteps);
}

void zero(TotalLengths lengths) {
  auto steps = inchToSteps(lengths);
  steppers[0].setCurrentPosition(steps.left);
  steppers[1].setCurrentPosition(-steps.right);
}

void setBearing(TruePosition end) {
  const auto start = getTruePosition();
  const auto delta = end - start;
  bearing = normalizeSpeed(delta);
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


double originOffset {10};  // in inches
}
