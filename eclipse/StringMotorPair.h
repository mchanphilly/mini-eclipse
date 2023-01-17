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
    delay(1000);

    isActive = true;
  }

  void disable() {
    assert(isActive);

    delay(1000);
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
    enable();
    left.step(1);
    left.step(-1);
    right.step(1);
    right.step(-1);
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