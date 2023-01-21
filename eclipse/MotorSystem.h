#ifndef MotorSystem_h
#define MotorSystem_h

#include <Stepper.h>
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
  Stepper left;
  Stepper right;

  // rotationsPerMinute 345 max
  static const int stepsPerRotation = 800;
  static const int maxRotationsPerMinute = 345;
  static const int rotationsPerMinute = 220;
  static_assert(rotationsPerMinute <= maxRotationsPerMinute);

  // Physical parameters
  static const int enableMotorPin = 8;
  static const double leftStepsPerInch = 331;
  static const double rightStepsPerInch = 331;

  // For timing
  // Spaced like this so we don't overflow.
  // micro/sec * rot/step * min/rot * sec/min
  // with current constants: 1E6 / 800 / 220 * 60
  static const int secondsPerMinute = 60;
  static const int microPerSecond = 1E6;
  static const double microPerStep = (double)microPerSecond / (double)stepsPerRotation / (double)rotationsPerMinute * (double)secondsPerMinute;

  bool isActive = false;

  // String position in steps (represents inches though)
  int lengths[2] = {8*leftStepsPerInch, 42.5*rightStepsPerInch};

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

  void inputToSteps(int* outSteps, double num1, double num2, Unit unit) {
    switch (unit) {
      case Unit::Inch:
          outSteps[0] = floor(leftStepsPerInch * num1);
          outSteps[1] = floor(rightStepsPerInch * num2);
          break;
      case Unit::Step:
          outSteps[0] = (int)num1;
          outSteps[1] = (int)num2;
          break;
    }
  }

  void setSpeed(long whatSpeed) {
    left.setSpeed(whatSpeed);
    right.setSpeed(whatSpeed);
  }

  public:

  MotorSystem(
              const int leftPin1,
              const int leftPin2,
              const int rightPin1,
              const int rightPin2):
    left(stepsPerRotation, leftPin1, leftPin2),
    right(stepsPerRotation, rightPin1, rightPin2)
    {
    }

  void init() {
    // "Warms up" the motors so that the first
    // movements aren't duds.

    const int initSteps = 1;
    const int initDelay = 200;
    setSpeed(rotationsPerMinute);

    enable();

    left.step(initSteps);
    right.step(initSteps);

    left.step(-initSteps);
    right.step(-initSteps);

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
    const int stepsPerStage = 1;
    int outSteps[2];

    enable();

    inputToSteps(outSteps, leftNum, rightNum, unit);
    int leftSteps = outSteps[0];
    int rightSteps = outSteps[1];

    // Notice the two signs are flipped.
    int leftSign = signbit(leftSteps) ? 1 : -1;
    int rightSign = signbit(rightSteps) ? -1 : 1;
    

    // /**
    //  * @brief New implementation: Allow the motors to finish at the same time.
    //  * 
    //  */

    
    // int maxSteps = max(abs(leftSteps), abs(rightSteps));
    // int minSteps = min(abs(leftSteps), abs(rightSteps));
    // int expectedTime = microPerStep * maxSteps;

    // double slowMicroPerStep = expectedTime / minSteps;
    
    // double highPace;
    // double lowPace;

    // int highStepsSoFar = 0;
    // int lowStepsSoFar = 0;
    // // TODO handle overflow
    // unsigned long startTime = micros();
    // unsigned long timeElapsed;

    /**
     * @brief Go through the timer and step when we're at or behind pace.
     * 
     */
    // while (leftSteps > 0 || rightSteps > 0) {
    //   // Time we expect to be by the time we take the next step.
    //   highPace = microPerStep * highStepsSoFar;
    //   lowPace = slowMicroPerStep * lowStepsSoFar;
      
    //   timeElapsed = micros() - startTime;

    //   if (highPace < timeElapsed) {

    //   }
    // }
    // const double stepsPerMicro = stepsPerMinute / microPerMinute;



    /**
     * @brief Old implementation: alternate steps then wrap up.
     * 
     */

    // Step together when the two motors can (one step per turn)
    Stepper *lesserStepper;
    Stepper *greaterStepper;

    int maxSteps = max(abs(leftSteps), abs(rightSteps));
    bool leftBigger = abs(leftSteps) > abs(rightSteps);


    int lesserSign, greaterSign;
    greaterStepper = leftBigger ? &left : &right;
    greaterSign = leftBigger ? leftSign : rightSign;
    lesserStepper = !leftBigger ? &left : &right;
    lesserSign = !leftBigger ? leftSign : rightSign;


    int sharedSteps = min(abs(leftSteps), abs(rightSteps));
    for (int i = 0; i < sharedSteps; i++) {
      left.step(leftSign);
      right.step(rightSign);
    }

    // Step separately for the remaining steps
    int stepsToGo = maxSteps - sharedSteps;
    greaterStepper->step(stepsToGo * greaterSign);

    // Old above

    // Track the stepping position
    lengths[0] += leftSteps;
    lengths[1] += rightSteps;

    disable();
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
    int desiredPosition[2];
    inputToSteps(desiredPosition, leftNum, rightNum, unit);

    // See what we need to do to get there from where we are.
    int leftToGo = desiredPosition[0] - lengths[0];
    int rightToGo = desiredPosition[1] - lengths[1];

    step(leftToGo, rightToGo, Unit::Step);
  }

  /**
   * @brief Reset the lengths of the strings (converts inches to steps)
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
          pair[0] = lengths[0] / leftStepsPerInch;
          pair[1] = lengths[1] / rightStepsPerInch;
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