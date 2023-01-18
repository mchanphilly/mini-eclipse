#ifndef StringMotor_h
#define StringMotor_h

#include <Stepper.h>
#include <assert.h>

class StringMotor {
    const float stepsPerInch = 325;
    Stepper motor;

    public:

    enum class Unit {
        Step,
        Inch
    };

    StringMotor(const int numSteps,
                const int pin1,
                const int pin2):
        motor(numSteps, pin1, pin2)
        {
        }

    /**
     * @brief Mildly hacky way of allowing both steps
     * and inches.
     * 
     * @param steps 
     * @param unit 
     */
    int step(float steps, Unit unit=Unit::Step) {
        int motorSteps;

        switch (unit) {
            case Unit::Inch:
                motorSteps = floor(stepsPerInch * steps);
                break;
            case Unit::Step:
                motorSteps = (int)steps;
                break;
        }

        motor.step(motorSteps);
        return motorSteps;
    }

    void setSpeed(long whatSpeed) {
        motor.setSpeed(whatSpeed);
    }
};

#endif