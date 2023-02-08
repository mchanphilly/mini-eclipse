#ifndef MotorSystem_h
#define MotorSystem_h

#include <AccelStepper.h>
#include "Lengths.h"

// Used as a wrapper for the Stepper, but with some added
// features.
namespace MotorSystem {
using namespace Lengths;

enum class Unit {
  Invalid,
  Step,
  Inch
};

struct Steps : public Printable {
  long left {0};
  long right {0};

  inline size_t printTo(Print& p) const {
    return p.print("Steps: ") + printToPair(p, left, right);
  }
};

void run(StringSpeed speed);

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

Steps getSteps();

extern const double stepsPerSecond;
extern const double inchRadius;

}

#endif