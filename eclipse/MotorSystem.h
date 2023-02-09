#ifndef MotorSystem_h
#define MotorSystem_h

#include <AccelStepper.h>
#include "Lengths.h"

// Used as a wrapper for the Stepper, but with some added
// features.
namespace MotorSystem {
using namespace Lengths;

void run(StringSpeed speed);

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

/**
 * @brief Reset the lengths of the strings from tangent to spool (converts inches to steps)
 * 
 */
void zero(TotalLengths lengths);

TotalLengths getLengths();

Steps getSteps();

Position getPosition();

Radial getRadial();

Tangential getTangential();

extern const double stepsPerSecond;
}

#endif