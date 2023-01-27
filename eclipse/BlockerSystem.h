#ifndef BlockerSystem_h
#define BlockerSystem_h

#include "MotorSystem.h"

/**
 * @brief Responsible for tracking the state of the physical system.
 * 
 */
class BlockerSystem {
    // All measurements done in inches from the contact point of the string on the spool.
    static constexpr double width = 41.232  ;  // Distance at motor level
    static constexpr double height = 55;  // Biggest permissible vertical displacement (less than longest string permitted)
    static constexpr double minHeight = 6;  // Limits max torque on spool; minimum permissible vertical displacement

    double originOffset;  // vertical offset from declared origin
    double currentPosition[2];  // x, y 

    void printBlockerPair(const String name, const double pair[2]) {
        Serial.print(name);
        Serial.print(": (");
        Serial.print(pair[0]);
        Serial.print(", ");
        Serial.print(pair[1]);
        Serial.println(")");
    }

    /**
     * @brief Use Heron's formula to find the vertical offset from the axis of the motors
     * 
     * @return double 
     */
    double findOffset(const double radials[2]) {        
        double semiPerimeter = (radials[0] + radials[1] + width) / 2;
        double area = sqrt(semiPerimeter *
                           (semiPerimeter - radials[0]) *
                           (semiPerimeter - radials[1]) *
                           (semiPerimeter - width));
        double offset = 2 * area / width;

        return offset;
    }

    // Uses radial lengths (mutates newPosition)
    void getPositionFromRadial(double outPosition[2], const double radial[2]) {        
        const double currentOffset = findOffset(radial);
        outPosition[0] = sqrt(sq(radial[0]) - sq(currentOffset));
        outPosition[1] = currentOffset - originOffset;
    }

    // Gets radial lengths (mutates radial)
    void getRadialFromPosition(double outRadial[2], const double position[2]) {
        const double currentOffset = position[1] + originOffset;
        outRadial[0] = sqrt(sq(position[0]) + sq(currentOffset));
        outRadial[1] = sqrt(sq(width - position[0]) + sq(currentOffset));
    }

    // Gets radial lengths from tangential lengths (mutates straightLength)
    void getRadialFromTangential(double outRadial[2], const double tangential[2]) {
        const auto radius = MotorSystem::inchRadius;
        for (int i = 0; i < 2; i++) {
            outRadial[i] = sqrt(sq(tangential[i]) + sq(radius));
        }
    }

    // Gets tangential lengths from radial lengths (mutates straightLength)
    void getTangentialFromRadial(double outTangential[2], const double radial[2]) {
        const auto radius = MotorSystem::inchRadius;
        for (int i = 0; i < 2; i++) {
            outTangential[i] = sqrt(sq(radial[i]) - sq(radius));
        }
    }

    void getPositionFromTangential(double outPosition[2], const double tangential[2]) {
        double radial[2];
        getRadialFromTangential(radial, tangential);
        getPositionFromRadial(outPosition, radial);
    }

    void getTangentialFromPosition(double outTangential[2], const double position[2]) {
        double radial[2];
        getRadialFromPosition(radial, position);
        getTangentialFromRadial(outTangential, radial);
    }

    /**
     * @brief Get the angle offsets in radians from the outer horizontal based on
     * position of the point.
     * 
     * @param offsets
     * @param position
     */
    void getAngleOffsetFromPosition(double angleOffsets[2], const double position[2]) {
        const double x = position[0];
        const double y = position[1] + originOffset;  // so we get true offset
        const double r = MotorSystem::inchRadius;
        printBlockerPair("Position", position);

        // We briefly need the radials again
        double radial[2];
        double tangential[2];
        getRadialFromPosition(radial, position);
        getTangentialFromPosition(tangential, position);

        printBlockerPair("Tangential", tangential);
        // Angle between radial and the vertical (careful for divide by zero)
        double verticalAngle[2];
        if (y > 0) {
            verticalAngle[0] = atan(x / y);
            verticalAngle[1] = atan((width - x) / y);            
        } else {
            verticalAngle[0] = 0;
            verticalAngle[1] = 0;
        }
        printBlockerPair("Vertical Angle", verticalAngle);

        // Angle between radial and the radius connected to tangent
        const double tangentialRadialAngle[2] = {
            acos(r/radial[0]),
            acos(r/radial[1])
        };

        printBlockerPair("RadTan corner", tangentialRadialAngle);

        // Offset is the difference between the horizontal and tangent
        for (int i = 0; i < 2; i++) {
            angleOffsets[i] = PI/2 - (tangentialRadialAngle[i] - verticalAngle[i]);
        }
        printBlockerPair("Final offsets", angleOffsets);
    }

    /**
     * @brief Get the string length of the arc on the spool from horizonal.
     * 
     * @param inchDeltas 
     * @param angleDeltas 
     */
    void getArcLength(double arcLengths[2], const double angleOffsets[2]) {
        for (int i = 0; i < 2; i++) {
            arcLengths[i] = angleOffsets[i] * MotorSystem::inchRadius;
        }
    }

    void getArcFromPosition(double arcLengths[2], const double position[2]) {
        double angleOffsets[2];
        getAngleOffsetFromPosition(angleOffsets, position);
        // Serial.println("Angle offset");
        // Serial.println(angleOffsets[0]);
        // Serial.println(angleOffsets[1]);
        getArcLength(arcLengths, angleOffsets);
    }

    void getArcFromTangential(double arcLengths[2], const double tangential[2]) {
        double position[2];
        getPositionFromTangential(position, tangential);
        getArcFromPosition(arcLengths, position);
    }

    public:
    BlockerSystem() {}

    /**
     * @brief Convert cartesian (x,y) on window into tangential string lengths in inches
     * 
     * @param x 
     * @param y  
     */
    void getLengthsFromPosition(double lengths[2], const double position[2]) {        
        double arcLengths[2];
        getTangentialFromPosition(lengths, position);
        getArcFromPosition(arcLengths, position);
        // Sum the tangential length and arc length.
        for (int i = 0; i < 2; i++) {
            lengths[i] += arcLengths[i];
        }
    }

    void getLengthsFromTangential(double lengths[2], const double tangential[2]) {
        double position[2];
        getPositionFromTangential(position, tangential);
        getLengthsFromPosition(lengths, position);
    }

    // // For debugging (checking position's guess of string length.)
    // void getTangentialFromLength(double lengths[2], const double tangentials[2]) {

    // }

    void getPosition(double pair[2]) {
        pair[0] = currentPosition[0];
        pair[1] = currentPosition[1];
    }

    void setPosition(const double pair[2]) {
        currentPosition[0] = pair[0];
        currentPosition[1] = pair[1];
        printBlockerPair("Set position to", pair);
    }

    // Gets the estimates for tangentials
    void getTangential(double buffer[2]) {
        getTangentialFromPosition(buffer, currentPosition);
        // Serial.println("Arc length");
        double arcLengths[2];
        getArcFromPosition(arcLengths, currentPosition);
        // Serial.println(arcLengths[0]);
        // Serial.println(arcLengths[1]);
        // Serial.println("From position");
        // Serial.println(currentPosition[0]);
        // Serial.println(currentPosition[1]);
        // Serial.println("Tangent length");
    }

    /**
     * @brief Reset internal position to origin and all that entails.
     * 
     * @param straightLengths
     */
    void hardZero(const double tangential[2]) {
        // We only need the radial lengths to find offset.
        double radial[2];
        getRadialFromTangential(radial, tangential);

        // Change origin so we offset appropriately.
        // TODO check if this just resets vertical but also horizontal
        // e.g. Can we reset 0,0 to be closer to the middle?
        originOffset = findOffset(radial);
        memset(currentPosition, 0, sizeof(currentPosition));
    }

    /**
     * @brief Reset internal position to reflect the physical system; the
     * origin remains the same as before.
     * 
     * @param straightLengths
     */
    void softZero(const double tangential[2]) {
        getPositionFromTangential(currentPosition, tangential);
    }
};

#endif