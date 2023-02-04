#ifndef BlockerSystem_h
#define BlockerSystem_h

#include <Arduino.h>
#include "MotorSystem.h"

/**
 * @brief Responsible for tracking the state of the physical system.
 * 
 */
class BlockerSystem {

public:
    class Position : public Printable {
    public:
        double x {0};
        double y {0};

        Position() = default;

        Position(double _x, double _y);

        size_t printTo(Print& p) const;
    };

    class StringPair : public Printable {
    public:
        double left {0};
        double right {0};

        StringPair() = default;

        StringPair(double _left, double _right);

        size_t printTo(Print& p) const;
    };

    /**
     * @brief Represents the radial distance from the center of the motors
     * to the blocker
     * 
     */
    class Radial : public StringPair {
    public:
        using StringPair::StringPair;
        /**
         * @brief Use Heron's formula to find the vertical offset from the axis of the motors
         * 
         * @return double 
         */
        double findOffset() const;
    };

    class Tangential : public StringPair {
    public:
        using StringPair::StringPair;
    };

    class ArcLength : public StringPair {
    public:
        using StringPair::StringPair;
    };

    class Angle : public StringPair {
    public:
        using StringPair::StringPair;

        Angle operator-(const Angle& other) const {
            const auto _left = this->left - other.left;
            const auto _right = this->right - other.right;
            const Angle out{_left, _right};
            return out;
        }

        inline ArcLength toArcLength(double radius) const {
            return ArcLength{this->left * radius, this->right * radius};
        }
    };

    class TotalLengths : public StringPair {
    public:
        using StringPair::StringPair;
        TotalLengths(Tangential tangential, ArcLength arc);
    };

    class StringState {
    public:
        StringState() = default;

        StringState(Radial _radial);

        StringState(Tangential _tangential);

        // Note that we don't yet update origin.
        // StringState(Position _position);

        void calibrate(Position _position);

        Radial toRadial() const;

        Tangential toTangential() const;

        Position toPosition() const;

        TotalLengths toTotalLengths() const;

    private:
        // Choosing radial for the internal representation.
        Radial radial;
        double originOffset;  // vertical offset from declared origin
    };

    BlockerSystem() = default;

    /**
     * @brief Return the internal string state
     */
    const StringState& getStringState() const;

    /**
     * @brief Reset internal position to origin and all that entails.
     * 
     * @param straightLengths
     */
    // void hardZero(const Tangential tangential);

    /**
     * @brief Reset internal position to reflect the physical system; the
     * origin remains the same as before.
     * 
     * @param straightLengths
     */
    void softZero(const Tangential tangential);

private:
    StringState state; 

    // All measurements done in inches from the contact point of the string on the spool.
    static constexpr double width {41.232};  // Distance at motor level
    static constexpr double height {55};  // Biggest permissible vertical displacement (less than longest string permitted)
    static constexpr double minHeight {6};  // Limits max torque on spool; minimum permissible vertical displacement
};

inline double getHypotenuse(double leg, double altitude) {
    return sqrt(sq(leg) + sq(altitude));
};

inline double getLeg(double hypotenuse, double leg) {
    return sqrt(sq(hypotenuse) - sq(leg));
}

template<class T>
inline T getHypotenuses(double leg1, double leg2, double altitude) {
    return T(getHypotenuse(leg1, altitude), getHypotenuse(leg2, altitude));
}

template<class T>
inline T getLegs(double hypotenuse1, double hypotenuse2, double altitude) {
    return T(getLeg(hypotenuse1, altitude), getLeg(hypotenuse2, altitude));
}

size_t printToPair(Print& p, double first, double second);

#endif
