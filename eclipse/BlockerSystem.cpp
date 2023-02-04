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

BlockerSystem::StringPair::StringPair(double _left, double _right)
    : left{_left}, right{_right}
    {}

size_t BlockerSystem::Position::printTo(Print& p) const {
    return printToPair(p, this->x, this->y);
}

size_t BlockerSystem::StringPair::printTo(Print& p) const {
    return printToPair(p, this->left, this->right);
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


void BlockerSystem::StringState::calibrate(Position _position) {
    const auto currentOffset =  _position.y + originOffset;
    radial = getHypotenuses<Radial>(_position.x, _position.y, currentOffset);
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
    return Position{x, y};
}

BlockerSystem::TotalLengths BlockerSystem::StringState::toTotalLengths() const {
    auto position = this->toPosition();
    const double x {position.x};
    const double y {position.y + originOffset};  // True y
    Serial.print("Position (not adjusted): ");
    Serial.println(position);

    // y should always be positive
    const Angle vertical {atan(x/y), atan((width - x)/y)};
    Serial.print("Vertical: ");
    Serial.println(vertical);

    // Angle between radial and the radius connected to tangent
    const double r {MotorSystem::inchRadius};
    const Angle tangentRadial {acos(r/radial.left), acos(r/radial.right)};
    Serial.print("RadTan corner: ");
    Serial.println(tangentRadial);

    const Angle starter{PI/2, PI/2};

    const Angle offset = starter - (tangentRadial - vertical);

    Serial.print("Offset angle: ");
    Serial.println(offset);

    const ArcLength arc = offset.toArcLength(r);

    return TotalLengths{this->toTangential(), arc};
}

const BlockerSystem::StringState& BlockerSystem::getStringState() const {
    return state;
}

// void BlockerSystem::hardZero(const Tangential tangential) {
//     // auto temp = StringState(tangential);
// }

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

    // /**
    //  * @brief Reset internal position to origin and all that entails.
    //  * 
    //  * @param straightLengths
    //  */
    // void hardZero(const double tangential[2]) {
    //     // We only need the radial lengths to find offset.
    //     double radial[2];
    //     getRadialFromTangential(radial, tangential);

    //     // Change origin so we offset appropriately.
    //     // TODO check if this just resets vertical but also horizontal
    //     // e.g. Can we reset 0,0 to be closer to the middle?
    //     originOffset = findOffset(radial);
    //     memset(currentPosition, 0, sizeof(currentPosition));
    // }