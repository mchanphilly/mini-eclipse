#ifndef Lengths_h
#define Lengths_h

#include <Arduino.h>

namespace Lengths {

// All measurements done in inches from the contact point of the string on the spool.
constexpr double minHeight {6};  // Limits max torque on spool; minimum permissible vertical displacement
constexpr double width {41.232};  // Distance at motor level

// Currently unused
constexpr double height {55};  // Biggest permissible vertical displacement (less than longest string permitted)

constexpr double inchPerRotation {2.37578};
constexpr double radius {inchPerRotation / (2*PI)};

struct GridPair : public Printable {
    double x {0};
    double y {0};

    GridPair() = default;

    GridPair(double pair[2]);

    GridPair(double _x, double _y);

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

struct Position;
struct TruePosition;

struct Radial;
struct Tangential;
struct TotalLengths;

struct Position : public GridPair {
    using GridPair::GridPair;

    // double offset {10};  // be sure to reassign
};

inline Position operator-(Position first, Position second) {
    return Position{second.x - first.x, second.y - first.y};
}

struct TruePosition : public GridPair {
    using GridPair::GridPair;
};

struct GridSpeed : public GridPair {
    using GridPair::GridPair;
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

    Radial(Tangential tangential);
    // Radial(Position position);
};

struct Tangential : public StringPair {
    using StringPair::StringPair;

    Tangential(Radial radial);
    // Tangential(Position position);
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

struct Steps : public Printable {
    long left {0};
    long right {0};

    Steps() = default;
    Steps(double pair[2]);
    Steps(long _left, long _right); 

    size_t printTo(Print& p) const;
};

}

#endif