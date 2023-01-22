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
  // left and right
  Stepper steppers[2];

  // rotationsPerMinute 345 max
  static constexpr int stepsPerRotation = 800;
  static constexpr int maxRotationsPerMinute = 345;
  static constexpr int rotationsPerMinute = 300;
  static_assert(rotationsPerMinute <= maxRotationsPerMinute);

  // Physical parameters
  static constexpr int enableMotorPin = 8;
  const double stepsPerInch[2] = {331, 331};


  static constexpr int microsPerSecond = 1e6;
  static constexpr int secondsPerMin = 60;

  // Precomputing the constant since 1E6 is beyond what we can operate on
  // micro/step = micro/sec * sec/min * min/rot * rot/step
  //              1E6       * 60      * 1/800   / steps/rot
  static constexpr double microsPerStepMultiplier = 75e3;
  static constexpr double microsPerStep = microsPerStepMultiplier / rotationsPerMinute;

  bool isActive = false;

  // String position in steps (represents inches though)
  int lengths[2] = {8*stepsPerInch[0], 42.5*stepsPerInch[1]};

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
          outSteps[0] = floor(stepsPerInch[0] * num1);
          outSteps[1] = floor(stepsPerInch[1] * num2);
          break;
      case Unit::Step:
          outSteps[0] = (int)num1;
          outSteps[1] = (int)num2;
          break;
    }
  }

  void setSpeed(long whatSpeed) {
    for (auto& stepper : steppers) {
      stepper.setSpeed(whatSpeed);
    }
  }

  public:

  MotorSystem(
              const int leftPin1,
              const int leftPin2,
              const int rightPin1,
              const int rightPin2):
    steppers{{stepsPerRotation, leftPin1, leftPin2}, {stepsPerRotation, rightPin1, rightPin2}}
    {
    }

  void init() {
    // "Warms up" the motors so that the first
    // movements aren't duds.

    const int initSteps = 1;
    const int initDelay = 200;
    setSpeed(rotationsPerMinute);

    enable();

    steppers[0].step(initSteps);
    steppers[1].step(initSteps);

    steppers[0].step(-initSteps);
    steppers[1].step(-initSteps);

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
    lengths[0] += outSteps[0];
    lengths[1] += outSteps[1];

    // Notice the two signs are flipped.
    int signs[2] = {
      signbit(outSteps[0]) ? 1 : -1,
      signbit(outSteps[1]) ? -1 : 1
    };
    

    // /**
    //  * @brief New implementation: Allow the motors to finish at the same time.
    //  * 
    //  */
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
    const int maxSteps = max(abs(outSteps[0]), abs(outSteps[1]));
    const int minSteps = min(abs(outSteps[0]), abs(outSteps[1]));

    const bool biggerIndex = abs(outSteps[0]) < abs(outSteps[1]);


    unsigned long maxTime = maxSteps * microsPerStep;
    unsigned long microsPerSmall = maxTime / minSteps;

    // Here we'll have the paces match the steppers (left and right)
    unsigned long paces[2] = {0};
    int stepsLeft[2] = {abs(outSteps[0]), abs(outSteps[1])};
    
    auto startTime = micros();
    while (stepsLeft[0] || stepsLeft[1]) {
        auto timeSoFar = micros() - startTime;
        // Serial.println(timeSoFar);
        
        if (timeSoFar > paces[biggerIndex] && stepsLeft[biggerIndex]) {
          paces[biggerIndex] += microsPerStep;
          stepsLeft[biggerIndex]--;
          steppers[biggerIndex].step(signs[biggerIndex]);
        }

        if (timeSoFar > paces[!biggerIndex] && stepsLeft[!biggerIndex]) {
          paces[!biggerIndex] += microsPerSmall;
          stepsLeft[!biggerIndex]--;
          steppers[!biggerIndex].step(signs[!biggerIndex]);
        }
    }
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
          pair[0] = lengths[0] / stepsPerInch[0];
          pair[1] = lengths[1] / stepsPerInch[1];
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