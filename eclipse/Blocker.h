#ifndef Blocker_h
#define Blocker_h

#include <Arduino.h>
#include "MotorSystem.h"

/**
 * @brief Responsible for tracking the state of the physical system.
 * 
 */
class Blocker {

public:
    struct Position : public Printable {
        double x {0};
        double y {0};

        Position() = default;

        Position(double pair[2]);

        Position(double _x, double _y);

        size_t printTo(Print& p) const;
    };

    struct StringPair : public Printable {
        double left {0};
        double right {0};

        StringPair() = default;

        StringPair(double pair[2]);

        StringPair(double _left, double _right);

        size_t printTo(Print& p) const;
    };

    /**
     * @brief Represents the radial distance from the center of the motors
     * to the blocker
     * 
     */
    struct Radial : public StringPair {
        using StringPair::StringPair;
        /**
         * @brief Use Heron's formula to find the vertical offset from the axis of the motors
         * 
         * @return double 
         */
        double findOffset() const;
    };

    struct Tangential : public StringPair {
        using StringPair::StringPair;
    };

    struct ArcLength : public StringPair {
        using StringPair::StringPair;
    };

    struct Angle : public StringPair {
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

    struct TotalLengths : public StringPair {
        using StringPair::StringPair;
        TotalLengths(Tangential tangential, ArcLength arc);
    };

    class StringState : public Printable {
    public:
        StringState() = default;

        StringState(Radial _radial);

        StringState(Tangential _tangential);

        // Note that we don't yet update origin.
        // StringState(Position _position);

        void update(Position _position);

        void softZero(Tangential tangential);

        void hardZero(Tangential tangential);

        Radial toRadial() const;

        Tangential toTangential() const;

        Position toPosition() const;

        TotalLengths toTotalLengths() const;

        size_t printTo(Print& p) const;

    private:
        // Choosing radial for the internal representation.
        Radial radial;
        double originOffset;  // vertical offset from declared origin
    };

    // Blocker(const StringState initialState);

    void update(Position position);

    /**
     * @brief Return the internal string state
     */
    const StringState& getStringState() const;

    /**
     * @brief Reset internal position to reflect the physical system; the
     * origin remains the same as before.
     * 
     * @param straightLengths
     */
    void softZero(Tangential tangential);

    void hardZero(Tangential tangential);

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
