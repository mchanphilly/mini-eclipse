#include "Blocker.h"

#include <Arduino.h>
#include "MotorSystem.h"

Blocker::Position::Position(double _x, double _y)
    : x{_x}, y{_y}
    {}

Blocker::Position::Position(double pair[2])
    : Position{pair[0], pair[1]}
    {}

Blocker::StringPair::StringPair(double _left, double _right)
    : left{_left}, right{_right}
    {}

Blocker::StringPair::StringPair(double pair[2])
    : StringPair{pair[0], pair[1]}
    {}

size_t Blocker::Position::printTo(Print& p) const {
    return p.print("Position: ") + printToPair(p, this->x, this->y);
}

size_t Blocker::StringPair::printTo(Print& p) const {
    return p.print("StringPair: ") + printToPair(p, this->left, this->right);
}

size_t Blocker::StringState::printTo(Print& p) const {
    return this->toTangential().printTo(p);
}

double Blocker::Radial::findOffset() const {
    const double semiPerimeter = (left + right + width) / 2;
    const double area = sqrt(semiPerimeter *
                        (semiPerimeter - left) *
                        (semiPerimeter - right) *
                        (semiPerimeter - width));
    const double offset {2 * area / width};

    return offset;
}

Blocker::TotalLengths::TotalLengths(Tangential tangential, ArcLength arc) {
    left = tangential.left + arc.left;
    right = tangential.right + arc.right;
}
    
Blocker::StringState::StringState(Radial _radial)
    : radial{_radial}, originOffset{_radial.findOffset()}
    {}

Blocker::StringState::StringState(Tangential _tangential) {
    const auto radius = MotorSystem::inchRadius;
    radial = getHypotenuses<Radial>(_tangential.left, _tangential.right, radius);
    originOffset = radial.findOffset();
}


void Blocker::StringState::update(Position _position) {
    const auto currentOffset =  _position.y + originOffset;
    radial = getHypotenuses<Radial>(_position.x, width - _position.x, currentOffset);
}

Blocker::Radial Blocker::StringState::toRadial() const {
    return radial;
}

Blocker::Tangential Blocker::StringState::toTangential() const {
    const auto radius = MotorSystem::inchRadius;
    return getLegs<Tangential>(radial.left, radial.right, radius);
}

Blocker::Position Blocker::StringState::toPosition() const {
    const double currentOffset = radial.findOffset();
    // Note that this assumes about the coordinate system.
    const double x = getLeg(radial.left, currentOffset);
    const double y = currentOffset - originOffset;
    return Position(x, y);
}

Blocker::TotalLengths Blocker::StringState::toTotalLengths() const {
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

void Blocker::update(Position position) {
    state.update(position);
}

const Blocker::StringState& Blocker::getStringState() const {
    return state;
}

void Blocker::softZero(const Tangential tangential) {
    state = StringState(tangential);
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
