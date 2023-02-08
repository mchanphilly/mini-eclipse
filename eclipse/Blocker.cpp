#include "Blocker.h"

#include <Arduino.h>
#include "MotorSystem.h"

namespace Blocker {
namespace {

// All measurements done in inches from the contact point of the string on the spool.
static constexpr double width {41.232};  // Distance at motor level
static constexpr double height {55};  // Biggest permissible vertical displacement (less than longest string permitted)
static constexpr double minHeight {6};  // Limits max torque on spool; minimum permissible vertical displacement

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

size_t printToPair(Print& p, double first, double second) {
    size_t size = 0;
    
    size += p.print("(");
    size += p.print(first);
    size += p.print(", ");
    size += p.print(second);
    size += p.print(")");

    return size;
}
}

GridPair::GridPair(double _x, double _y)
    : x{_x}, y{_y}
    {}

GridPair::GridPair(double pair[2])
    : GridPair{pair[0], pair[1]}
    {}

StringPair::StringPair(double _left, double _right)
    : left{_left}, right{_right}
    {}

StringPair::StringPair(double pair[2])
    : StringPair{pair[0], pair[1]}
    {}

size_t GridPair::printTo(Print& p) const {
    return p.print("GridPair: ") + printToPair(p, this->x, this->y);
}

size_t StringPair::printTo(Print& p) const {
    return p.print("StringPair: ") + printToPair(p, this->left, this->right);
}

size_t StringState::printTo(Print& p) const {
    return this->toTangential().printTo(p);
}

double Radial::findOffset() const {
    const double semiPerimeter = (left + right + width) / 2;
    const double area = sqrt(semiPerimeter *
                        (semiPerimeter - left) *
                        (semiPerimeter - right) *
                        (semiPerimeter - width));
    const double offset {2 * area / width};

    return offset;
}

TotalLengths::TotalLengths(Tangential tangential, ArcLength arc) {
    left = tangential.left + arc.left;
    right = tangential.right + arc.right;
}
    
StringState::StringState(Radial _radial)
    : radial{_radial}, originOffset{_radial.findOffset()}
    {}

StringState::StringState(Tangential _tangential) {
    const auto radius = MotorSystem::inchRadius;
    radial = getHypotenuses<Radial>(_tangential.left, _tangential.right, radius);
    originOffset = radial.findOffset();
}


void StringState::update(Position _position) {
    const auto currentOffset =  _position.y + originOffset;
    radial = getHypotenuses<Radial>(_position.x, width - _position.x, currentOffset);
}

void StringState::softZero(Tangential tangential) {
    const auto tempState = StringState(tangential);
    radial = tempState.toRadial();
}

void StringState::hardZero(Tangential tangential) {
    const auto tempState = StringState(tangential);
    // Must be a better way than this;
    radial = tempState.radial;
    originOffset = tempState.originOffset;
}

Radial StringState::toRadial() const {
    return radial;
}

Tangential StringState::toTangential() const {
    const auto radius = MotorSystem::inchRadius;
    return getLegs<Tangential>(radial.left, radial.right, radius);
}

Position StringState::toPosition() const {
    const double currentOffset = radial.findOffset();
    // Note that this assumes about the coordinate system.
    const double x = getLeg(radial.left, currentOffset);
    const double y = currentOffset - originOffset;
    return Position(x, y);
}

TotalLengths StringState::toTotalLengths() const {
    auto position = this->toPosition();
    const double x {position.x};
    const double y {position.y + originOffset};  // True y

    // y should always be positive
    const Angle vertical {atan(x/y), atan((width - x)/y)};

    // Angle between radial and the radius connected to tangent
    const double r {MotorSystem::inchRadius};
    const Angle tangentRadial {acos(r/radial.left), acos(r/radial.right)};
    const Angle starter{PI/2, PI/2};
    const Angle offset = starter - (tangentRadial - vertical);

    const ArcLength arc = offset.toArcLength(r);

    return TotalLengths{this->toTangential(), arc};
}
}
