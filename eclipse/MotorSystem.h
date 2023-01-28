#ifndef MotorSystem_h
#define MotorSystem_h

#include <AccelStepper.h>
#include <assert.h>

class MotorSystem {
  // Used as a wrapper for the Stepper, but with some added
  // features.

  public:
    enum class Unit {
      Invalid,
      Step,
      Inch
    };

  private:
  // left and right
  AccelStepper steppers[2];

  static constexpr int enableMotorPin = 8;

  static constexpr int stepsInMotor = 200;
  static constexpr int microsteps = 8;
  static constexpr double stepsPerRotation = stepsInMotor * microsteps;

  static constexpr int secondsPerMinute = 60;

  static constexpr double maxRotationsPerMinute = 500;
  static constexpr double maxRotationsPerSecond = maxRotationsPerMinute / secondsPerMinute;
  static constexpr double maxStepsPerSecond = maxRotationsPerSecond * stepsPerRotation;

  static constexpr double rotationsPerMinute = 320;
  static_assert(rotationsPerMinute <= maxRotationsPerMinute);
  static constexpr double rotationsPerSecond = rotationsPerMinute / secondsPerMinute;

  static constexpr double stepsPerSecond = rotationsPerSecond * stepsPerRotation;

  // Parameters of the stepper motors.

  static constexpr double stepRadius = (double)stepsPerRotation / (2*PI);

  // // Parameters of the string set-up (including initial conditions)
  static constexpr double inchPerRotation = 2.37578;
  static constexpr double stepsPerInch = stepsPerRotation / inchPerRotation;

  // String position in steps offset from the horizontal position, not tangential or radial length.
  long int lengths[2];

  void enable() {
    steppers[1].enableOutputs();
    delay(100);  // Doesn't impact motor stuttering
  }

  void disable() {
    delay(50);
    steppers[0].disableOutputs();
  }

  void inputToSteps(long* outSteps, double num1, double num2, Unit unit) {
    switch (unit) {
      case Unit::Inch:
          outSteps[0] = lround(stepsPerInch * num1);
          outSteps[1] = lround(stepsPerInch * num2);
          break;
      case Unit::Step:
          outSteps[0] = (long)num1;
          outSteps[1] = (long)num2;
          break;
    }
  }

  void setSpeed(long whatSpeed) {
    for (auto& stepper : steppers) {
      stepper.setSpeed(whatSpeed);
    }
  }

  public:
  static constexpr double inchRadius = stepRadius / stepsPerInch;

  MotorSystem(
              const int leftPin1,
              const int leftPin2,
              const int rightPin1,
              const int rightPin2):
    steppers{{AccelStepper::DRIVER, leftPin1, leftPin2}, {AccelStepper::DRIVER, rightPin1, rightPin2}}
    {
    }

  void run() {
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

  /**
   * @brief Command the motors to step a certain number of steps
   * or inches from the current position.
   * 
   * @param leftNum 
   * @param rightNum 
   * @param unit 
   */
  void step(double leftNum, double rightNum, Unit unit) {
    enable();

    long outSteps[2];
    inputToSteps(outSteps, leftNum, rightNum, unit);
    lengths[0] += outSteps[0];
    lengths[1] += outSteps[1];

    steppers[0].move(outSteps[0]);
    steppers[0].setSpeed(stepsPerSecond);
    steppers[1].move(-outSteps[1]);
    steppers[1].setSpeed(stepsPerSecond);
  }

  /**
   * @brief Command the motors to step to a certain position from
   * the zero position.
   * 
   * @param leftNum 
   * @param rightNum 
   * @param unit 
   */
  void go(double leftNum, double rightNum, Unit unit) {
    // Converting to steps first
    long desiredPosition[2];
    inputToSteps(desiredPosition, leftNum, rightNum, unit);

    // See what we need to do to get there from where we are.
    long leftToGo = desiredPosition[0] - lengths[0];
    long rightToGo = desiredPosition[1] - lengths[1];

    step(leftToGo, rightToGo, Unit::Step);
  }

  /**
   * @brief Reset the lengths of the strings from tangent to spool (converts inches to steps)
   * 
   * @param leftLength 
   * @param rightLength 
   */
  void zero(double leftLength, double rightLength) {
    inputToSteps(lengths, leftLength, rightLength, Unit::Inch);
  }

  void getLengths(double pair[2], Unit unit) {
    // Serial.println("New lengths?");
    // Serial.println(lengths[0]);
    // Serial.println(lengths[1]);

    switch (unit) {
        case Unit::Inch:
          pair[0] = lengths[0] / stepsPerInch;
          pair[1] = lengths[1] / stepsPerInch;
          break;
        case Unit::Step:
          // TODO I'm sure there's a better way to write this.
          pair[0] = lengths[0];
          pair[1] = lengths[1];
          break;
    }
  }
};

#endif