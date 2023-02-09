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

const StringSpeed defaultSpeed{stepsPerSecond, stepsPerSecond};
const GridSpeed defaultBearing{gridLimit, gridLimit};
constexpr unsigned long speedPace{1000};
unsigned long currentTime{0};

// StringSpeed lastSpeed;
GridSpeed bearing{gridLimit, gridLimit};
StringSpeed currentSpeed {stepsPerSecond, stepsPerSecond};

StringSpeed getSpeed() {
    // Expects that the speed variable is updated for current trajectory.
    const Tangential tangential = Tangential(getLengths());
    const TruePosition truePosition = TruePosition(Radial(tangential));

    const auto rateFactor = truePosition.x * bearing.x + truePosition.y * bearing.y;

    const StringSpeed stringSpeed {
        rateFactor / tangential.left,
        (rateFactor - width * bearing.x) / tangential.right
    };
    // Serial.println(stringSpeed);
    return stringSpeed;
}

void enable() {
  steppers[1].enableOutputs();
  delay(100);  // Doesn't impact motor stuttering
}

void disable() {
  delay(50);
  steppers[0].disableOutputs();
  bearing = defaultBearing;  // reset bearing
  currentSpeed = defaultSpeed;
}

Steps inchToSteps(TotalLengths lengths) {
  Steps outSteps;
  outSteps.left = lround(stepsPerInch * lengths.left);
  outSteps.right = lround(stepsPerInch * lengths.right);
  return outSteps;
}

inline bool stillMoving() {
  return steppers[0].distanceToGo() || steppers[1].distanceToGo();
}

}

void run() {
  if (millis() - currentTime > 100) {
    currentSpeed = getSpeed();
    Serial.println(currentSpeed);
    currentTime = millis();
  }

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
  // currentSpeed = maxSpeed;
}

void go(TotalLengths lengths) {
  auto steps = inchToSteps(lengths);
  go(steps);
}

void go(Steps steps) {
  enable();
  steppers[0].moveTo(steps.left);
  steppers[1].moveTo(-steps.right);
  // currentSpeed = getSpeed();
}

void zero(TotalLengths lengths) {
  auto steps = inchToSteps(lengths);
  steppers[0].setCurrentPosition(steps.left);
  steppers[1].setCurrentPosition(-steps.right);
}

// todo consider caching previous TruePosition target
void setBearing(TruePosition end) {
  const TruePosition start = TruePosition(Radial(Tangential(getLengths())));
  const TruePosition delta = end - start;
  // Scale so that the bigger component moves at max grid speed.
  const double scaleFactor {gridLimit / max(delta.x, delta.y)};
  bearing = GridSpeed(scaleFactor * delta.x, scaleFactor * delta.y);
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
  Serial.println("Position???");
  Serial.println(getRadial());
  Serial.println(TruePosition(getRadial()));
  // Serial.println(Position(TruePosition(getRadial()), originOffset));
  return Position(TruePosition(getRadial()), originOffset);
}

Radial getRadial() {
  return Radial(getTangential());
}

Tangential getTangential() {
  return Tangential(getLengths());
}

double originOffset {10};  // in inches
}