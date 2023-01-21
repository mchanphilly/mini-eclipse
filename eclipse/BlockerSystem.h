#ifndef BlockerSystem_h
#define BlockerSystem_h

#include "MotorSystem.h"

/**
 * @brief Responsible for tracking the state of the physical system.
 * 
 */
class BlockerSystem {
    // All measurements done in inches from the contact point of the string on the spool.
    const double width = 41.75  ;  // Distance at motor level
    const double height = 55;  // Biggest permissible vertical displacement (less than longest string permitted)
    const double minHeight = 6;  // Limits max torque on spool; minimum permissible vertical displacement

    double originOffset;  // vertical offset from declared origin
    double currentPosition[2];  // x, y 

    /**
     * @brief Use Heron's formula to find the vertical offset from the axis of the motors
     * 
     * @return double 
     */
    double findOffset(double lengths[2]) {        
        Serial.println("Lengths in Offset:");
        Serial.println(lengths[0]);
        Serial.println(lengths[1]);

        double semiPerimeter = (lengths[0] + lengths[1] + width) / 2;
        double area = sqrt(semiPerimeter *
                           (semiPerimeter - lengths[0]) *
                           (semiPerimeter - lengths[1]) *
                           (semiPerimeter - width));
        double offset = 2 * area / width;

        return offset;
    }

    public:
    BlockerSystem() {}

    /**
     * @brief Convert cartesian (x,y) on window into string lengths in inches
     * 
     * @param x 
     * @param y  
     */
    void getLengthsFromPosition(double lengths[2], double x, double y) {
        lengths[0] = sqrt(sq(x) + sq(y + originOffset));
        lengths[1] = sqrt(sq(width - x) + sq(y + originOffset));
        
        currentPosition[0] = x;
        currentPosition[1] = y;
    }

    void getPosition(double pair[2]) {
        pair = this->currentPosition;
    }

    void zero(double lengths[2]) {
        memset(currentPosition, 0, sizeof(currentPosition));
        originOffset = findOffset(lengths);
    }
};

#endif