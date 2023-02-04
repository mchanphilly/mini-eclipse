#include "BlockerSystem.h"

#include <Arduino.h>
#include "MotorSystem.h"

// using BlockerSystem::StringState;
// using BlockerSystem::Radial;
// using BlockerSystem::Tangential;
// using BlockerSystem::Position;
// using namespace BlockerSystem;

BlockerSystem::Position::Position(double _x, double _y)
    : x{_x}, y{_y}
    {}

BlockerSystem::Position::Position(double pair[2])
    : Position{pair[0], pair[1]}
    {}

BlockerSystem::StringPair::StringPair(double _left, double _right)
    : left{_left}, right{_right}
    {}

size_t BlockerSystem::Position::printTo(Print& p) const {
    return printToPair(p, this->x, this->y);
}

size_t BlockerSystem::StringPair::printTo(Print& p) const {
    return printToPair(p, this->left, this->right);
}

size_t BlockerSystem::StringState::printTo(Print& p) const {
    return this->toTangential().printTo(p);
}

double BlockerSystem::Radial::findOffset() const {
    const double semiPerimeter = (left + right + width) / 2;
    const double area = sqrt(semiPerimeter *
                        (semiPerimeter - left) *
                        (semiPerimeter - right) *
                        (semiPerimeter - width));
    const double offset {2 * area / width};

    return offset;
}

BlockerSystem::TotalLengths::TotalLengths(Tangential tangential, ArcLength arc) {
    left = tangential.left + arc.left;
    right = tangential.right + arc.right;
}
    
BlockerSystem::StringState::StringState(Radial _radial)
    : radial{_radial}, originOffset{_radial.findOffset()}
    {}

BlockerSystem::StringState::StringState(Tangential _tangential) {
    const auto radius = MotorSystem::inchRadius;
    radial = getHypotenuses<Radial>(_tangential.left, _tangential.right, radius);
    originOffset = radial.findOffset();
}


void BlockerSystem::StringState::update(Position _position) {
    const auto currentOffset =  _position.y + originOffset;
    radial = getHypotenuses<Radial>(_position.x, width - _position.x, currentOffset);
}

BlockerSystem::Radial BlockerSystem::StringState::toRadial() const {
    return radial;
}

BlockerSystem::Tangential BlockerSystem::StringState::toTangential() const {
    const auto radius = MotorSystem::inchRadius;
    return getLegs<Tangential>(radial.left, radial.right, radius);
}

BlockerSystem::Position BlockerSystem::StringState::toPosition() const {
    const double currentOffset = radial.findOffset();
    // Note that this assumes about the coordinate system.
    const double x = getLeg(radial.left, currentOffset);
    const double y = currentOffset - originOffset;
    return Position(x, y);
}

BlockerSystem::TotalLengths BlockerSystem::StringState::toTotalLengths() const {
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

void BlockerSystem::update(Position position) {
    state.update(position);
}

const BlockerSystem::StringState& BlockerSystem::getStringState() const {
    return state;
}

void BlockerSystem::softZero(const Tangential tangential) {
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
