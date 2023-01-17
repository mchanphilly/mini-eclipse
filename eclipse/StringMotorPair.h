#ifndef StringMotorPair_h
#define StringMotorPair_h

#include <Stepper.h>
#include <assert.h>

class StringMotorPair {
  // Used as a wrapper for the Stepper, but with some added
  // features.
  Stepper left;
  Stepper right;
  static const int enableMotorPin = 8;
  bool isActive = false;

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
  StringMotorPair(const int numSteps,
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

    left.step(initSteps);
    right.step(initSteps);

    delay(initDelay);

    left.step(-initSteps);
    right.step(-initSteps);

    disable();
  }

  void step(int leftSteps, int rightSteps) {
    enable();
    left.step(leftSteps);
    right.step(rightSteps);
    disable();
  }

  void setSpeed(long whatSpeed) {
    left.setSpeed(whatSpeed);
    right.setSpeed(whatSpeed);
  }
};

#endif