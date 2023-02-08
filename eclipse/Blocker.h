#ifndef Blocker_h
#define Blocker_h

#include <Arduino.h>
#include "MotorSystem.h"

/**
 * @brief Responsible for tracking the state of the physical system.
 * 
 */
namespace Blocker {

struct GridPair : public Printable {
    double x {0};
    double y {0};

    GridPair() = default;

    GridPair(double pair[2]);

    GridPair(double _x, double _y);

    size_t printTo(Print& p) const;
};

struct Position : public GridPair {
    using GridPair::GridPair;
};

struct GridSpeed : public GridPair {
    using GridPair::GridPair;
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

struct StringSpeed : public StringPair {
    using StringPair::StringPair;
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
}

#endif
