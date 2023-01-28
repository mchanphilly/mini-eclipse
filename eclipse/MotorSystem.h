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

  static constexpr double rotationsPerMinute = 240;
  static_assert(rotationsPerMinute <= maxRotationsPerMinute);
  static constexpr double rotationsPerSecond = rotationsPerMinute / secondsPerMinute;

  static constexpr double stepsPerSecond = rotationsPerSecond * stepsPerRotation;

  // Parameters of the stepper motors.
  // static constexpr int enableMotorPin = 8;
  // static constexpr int stepsInMotor = 200;
  // static constexpr int stepsPerRotation = stepsInMotor * 128;  // Account for microsteps
  // static constexpr int maxRotationsPerMinute = 500;
  static constexpr double stepRadius = (double)stepsPerRotation / (2*PI);

  // // User sets this.
  // static constexpr int rotationsPerMinute = 500 * 2;  // Workaround because we can't set steps in the Stepper to be bigger than fits in int.
  // static_assert(rotationsPerMinute <= maxRotationsPerMinute * 2);

  // // Parameters of the string set-up (including initial conditions)
  // static constexpr double inchPerRotation = 2.4169;
  static constexpr double inchPerRotation = 2.37578;
  static constexpr double stepsPerInch = stepsPerRotation / inchPerRotation;

  // // Unit constants
  // static constexpr int microsPerSecond = 1e6;
  // static constexpr int secondsPerMin = 60;

  // Precomputing the constant since 1E6 is beyond what we can operate on
  // micro/step = micro/sec * sec/min * min/rot * rot/step
  //              1E6       * 60      * 1/800   / steps/rot
  // static constexpr double microsPerStepMultiplier = 75e3;
  // static constexpr double microsPerStep = microsPerStepMultiplier / rotationsPerMinute;

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
      // stepper.setAcceleration(1);
    }


    // steppers[0].setSpeed(stepsPerSecond);

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

    // Notice the two signs are flipped according to how the string is spooled.
    const int signs[2] = {
      signbit(outSteps[0]) ? -1 : 1,
      signbit(outSteps[1]) ? 1 : -1
    };
    
    const bool biggerIndex = abs(outSteps[0]) < abs(outSteps[1]);
    long stepsLeft[2] = {abs(outSteps[0]), abs(outSteps[1])};
    Serial.println(outSteps[0]);

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