#ifndef MotorSystem_h
#define MotorSystem_h

#include <AccelStepper.h>
#include "Lengths.h"

// Used as a wrapper for the Stepper, but with some added
// features.
namespace MotorSystem {
using namespace Lengths;

void run();

void init(Tangential tangential);

/**
 * @brief Command the motors to step a certain number of steps
 * or inches from the current position.
 * 
 * @param leftNum 
 * @param rightNum 
 * @param unit 
 */
void step(TotalLengths lengths);

void step(Steps steps);

/**
 * @brief Command the motors to step to a certain position from
 * the zero position.
 * 
 */
void go(TotalLengths lengths);

void go(Steps steps);

// void setOffset(double offset);

/**
 * @brief Reset the lengths of the strings from tangent to spool (converts inches to steps)
 * 
 */
void zero(TotalLengths lengths);

void setBearing(TruePosition end);

bool stillMoving();

TotalLengths getLengths();

Steps getSteps();

extern const double stepsPerSecond;
extern double originOffset;

inline Tangential getTangential() {
  return Tangential(getLengths());
}

inline Radial getRadial() {
  return Radial(getTangential());
}

inline TruePosition getTruePosition() {
  return TruePosition(getRadial());
}

inline Position getPosition() {
  return Position(getTruePosition(), originOffset);
}

}

#endif
