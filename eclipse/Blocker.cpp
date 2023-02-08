#include "Blocker.h"

#include <Arduino.h>
#include "MotorSystem.h"

namespace Blocker {
namespace {

// All measurements done in inches from the contact point of the string on the spool.
static constexpr double width {41.232};  // Distance at motor level
static constexpr double height {55};  // Biggest permissible vertical displacement (less than longest string permitted)
static constexpr double minHeight {6};  // Limits max torque on spool; minimum permissible vertical displacement

// Assume speed limits are the same for both motors.
static constexpr double slowFactor {0.9};
static_assert(0 < slowFactor && slowFactor <= 1);
static constexpr double gridLimit {slowFactor / sqrt(2) * MotorSystem::stepsPerSecond};

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

GridSpeed getTrajectory(Position start, Position end) {
    // Might be abuse of notation unless you consider Position as a vector
    const Position delta {end - start};
    // Scale so that the bigger component moves at max grid speed.
    const double scaleFactor {gridLimit / max(delta.x, delta.y)};
    return GridSpeed{scaleFactor * delta.x, scaleFactor * delta.y};
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


void StringState::go(Position _position) {
    /**
     * @brief Do two things:
     * - Update gridSpeed to what we need it to be
     * - Update radial to the new position. (!!! issue)
     *  TODO figure out how to synchronize Blocker position
     *  and MotorSystem's position; prob with AccelStepper or MotorSystem
     *  tracker
     * 
     */
    // const auto start = this->toPosition();
    const auto currentOffset =  _position.y + originOffset;
    radial = getHypotenuses<Radial>(_position.x, width - _position.x, currentOffset);
    // Get trajectory too
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

StringSpeed StringState::getSpeed() const {
    // Expects that the speed variable is updated for current trajectory.
    auto position = this->toTruePosition();
    auto tangential = this->toTangential();
    const auto rateFactor = position.x * speed.x + position.y * speed.y;

    const StringSpeed stringSpeed {
        rateFactor / tangential.left,
        (rateFactor - width * speed.x) / tangential.right
    };

    return stringSpeed;
}

Radial StringState::toRadial() const {
    return radial;
}

Tangential StringState::toTangential() const {
    const auto radius = MotorSystem::inchRadius;
    return getLegs<Tangential>(radial.left, radial.right, radius);
}

Position StringState::toPosition() const {
    const auto truePosition = this->toTruePosition();
    return Position{truePosition.x, truePosition.y - originOffset};
}

TruePosition StringState::toTruePosition() const {
    const double currentOffset = radial.findOffset();
    // Note that this assumes about the coordinate system.
    const double x = getLeg(radial.left, currentOffset);
    const double y = currentOffset;
    return TruePosition(x, y);
}

TotalLengths StringState::toTotalLengths() const {
    auto position = this->toTruePosition();

    // y should always be positive
    const Angle vertical {
        atan(position.x/position.y),
        atan((width - position.x)/position.y)
    };

    // Angle between radial and the radius connected to tangent
    const double r {MotorSystem::inchRadius};
    const Angle tangentRadial {acos(r/radial.left), acos(r/radial.right)};
    const Angle starter{PI/2, PI/2};
    const Angle offset = starter - (tangentRadial - vertical);

    const ArcLength arc = offset.toArcLength(r);

    return TotalLengths{this->toTangential(), arc};
}
}
