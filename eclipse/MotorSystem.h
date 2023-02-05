#ifndef MotorSystem_h
#define MotorSystem_h

#include <AccelStepper.h>
#include <assert.h>

// Used as a wrapper for the Stepper, but with some added
// features.
class MotorSystem {
public:
  enum class Unit {
    Invalid,
    Step,
    Inch
  };

  MotorSystem();

  void run();

  void init();

  /**
   * @brief Command the motors to step a certain number of steps
   * or inches from the current position.
   * 
   * @param leftNum 
   * @param rightNum 
   * @param unit 
   */
  void step(double leftNum, double rightNum, Unit unit);

  /**
   * @brief Command the motors to step to a certain position from
   * the zero position.
   * 
   * @param leftNum 
   * @param rightNum 
   * @param unit 
   */
  void go(double leftNum, double rightNum, Unit unit);

  /**
   * @brief Reset the lengths of the strings from tangent to spool (converts inches to steps)
   * 
   * @param leftLength 
   * @param rightLength 
   */
  void zero(double leftLength, double rightLength);

  void getLengths(double pair[2], Unit unit);

private:
  // left and right
  AccelStepper steppers[2];

  static constexpr int stepXPin {2};
  static constexpr int dirXPin {5};
  static constexpr int stepYPin {3};
  static constexpr int dirYPin {6};
  static constexpr int enableMotorPin {8};

  static constexpr int stepsInMotor {200};
  static constexpr int microsteps {8};
  static constexpr double stepsPerRotation {stepsInMotor * microsteps};

  static constexpr int secondsPerMinute {60};

  static constexpr double maxRotationsPerMinute {500};
  static constexpr double maxRotationsPerSecond {maxRotationsPerMinute / secondsPerMinute};
  static constexpr double maxStepsPerSecond {maxRotationsPerSecond * stepsPerRotation};

  static constexpr double rotationsPerMinute {320};
  static_assert(rotationsPerMinute <= maxRotationsPerMinute);
  static constexpr double rotationsPerSecond {rotationsPerMinute / secondsPerMinute};

  static constexpr double stepsPerSecond {rotationsPerSecond * stepsPerRotation};

  // Parameters of the stepper motors.

  static constexpr double stepRadius = {stepsPerRotation / (2*PI)};

  // // Parameters of the string set-up (including initial conditions)
  static constexpr double inchPerRotation {2.37578};
  static constexpr double stepsPerInch {stepsPerRotation / inchPerRotation};

  // String position in steps offset from the horizontal position, not tangential or radial length.
  long int lengths[2];

  void enable();

  void disable();

  void inputToSteps(long* outSteps, double num1, double num2, Unit unit);

  void setSpeed(long whatSpeed);
  
// Would've preferred to have all the public things together, but this depends on private variables.
public:
  static constexpr double inchRadius {stepRadius / stepsPerInch};
};

#endif