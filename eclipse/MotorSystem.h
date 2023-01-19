#ifndef MotorSystem_h
#define MotorSystem_h

#include <Stepper.h>
#include <assert.h>

#include "StringMotor.h"

class MotorSystem {
  // Used as a wrapper for the Stepper, but with some added
  // features.
  StringMotor left;
  StringMotor right;
  static const int enableMotorPin = 8;

  bool isActive = false;

  int steps[2] = {0};

  void enable() {
    assert(!isActive);
    digitalWrite(enableMotorPin, LOW);
    isActive = true;
  }

  void disable() {
    assert(isActive);

    // Necessary so that the momentum is brought to 0 before power is off.
    delay(50);  // This number can be reduced if speed is lower, but 30ms is good for 300RPM. 50ms to be safe.
    digitalWrite(enableMotorPin, HIGH);
    isActive = false;
  }

  public:
  MotorSystem(
              const int numSteps,
              const int leftPin1,
              const int leftPin2,
              const int rightPin1,
              const int rightPin2):
    left(numSteps, leftPin1, leftPin2),
    right(numSteps, rightPin1, rightPin2)
    {
    }

  void init() {
    // "Warms up" the motors so that the first
    // movements aren't duds.

    const int initSteps = 1;
    const int initDelay = 200;

    enable();

    delay(initDelay);

    // TODO parallelize (not too big a priority)
    left.step(initSteps);
    right.step(initSteps);

    delay(initDelay);

    left.step(-initSteps);
    right.step(-initSteps);

    disable();
  }

  void step(float leftSteps, float rightSteps, StringMotor::Unit unit) {
    enable();
    steps[0] -= left.step(-leftSteps, unit);
    steps[1] += right.step(rightSteps, unit);
    disable();
  }

  void setSpeed(long whatSpeed) {
    left.setSpeed(whatSpeed);
    right.setSpeed(whatSpeed);
  }

  void zero() {
    memset(steps, 0, sizeof(steps));
  }

  int* getSteps() {
    return this->steps;
  }
};

#endif