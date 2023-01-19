#ifndef MotorSystem_h
#define MotorSystem_h

#include <Stepper.h>
#include <assert.h>

class MotorSystem {
  // Used as a wrapper for the Stepper, but with some added
  // features.
  Stepper left;
  Stepper right;
  static const int enableMotorPin = 8;
  const float leftStepsPerInch = 325;
  const float rightStepsPerInch = 325;

  bool isActive = false;

  int steps[2] = {0};

  void enable() {
    assert(!isActive);
    digitalWrite(enableMotorPin, LOW);
    isActive = true;
    delay(100);  // Doesn't impact motor stuttering
  }

  void disable() {
    assert(isActive);

    // Necessary so that the momentum is brought to 0 before power is off.
    delay(50);  // This number can be reduced if speed is lower, but 30ms is good for 300RPM. 50ms to be safe.
    digitalWrite(enableMotorPin, HIGH);
    isActive = false;
  }

  public:
  enum class Unit {
    Step,
    Inch
  };
  
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

    left.step(initSteps);
    right.step(initSteps);

    left.step(-initSteps);
    right.step(-initSteps);

    disable();
  }

  void step(float leftNum, float rightNum, Unit unit) {
    const int stepsPerStage = 1;
    int leftSteps;
    int rightSteps;

    enable();

    // do a jank threading sort of deal
    switch (unit) {
        case Unit::Inch:
            leftSteps = floor(leftStepsPerInch * leftNum);
            rightSteps = floor(rightStepsPerInch * rightNum);
            break;
        case Unit::Step:
            leftSteps = (int)leftNum;
            rightSteps = (int)rightNum;
            break;
    }
    int leftSign = signbit(leftSteps) ? -1 : 1;
    int rightSign = signbit(rightSteps) ? -1 : 1;
    
    int sharedSteps = min(abs(leftSteps), abs(rightSteps));
    for (int i = 0; i < sharedSteps; i++) {
      left.step(-leftSign);
      right.step(rightSign);
    }

    int stepsToGo;
    if (abs(leftSteps) > abs(rightSteps)) {
      stepsToGo = abs(leftSteps) - sharedSteps;
      left.step(-stepsToGo * leftSign);
    } else {
      stepsToGo = abs(rightSteps) - sharedSteps;
      right.step(stepsToGo * rightSign);
    }


    steps[0] += leftSteps;
    steps[1] += rightSteps;
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