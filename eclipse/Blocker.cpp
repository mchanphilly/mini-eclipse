#include "Blocker.h"
#include "Lengths.h"
#include <Arduino.h>
#include "MotorSystem.h"

namespace Blocker {
using namespace Lengths;
namespace {

// Assume speed limits are the same for both motors.
static constexpr double slowFactor {0.9};
static_assert(0 < slowFactor && slowFactor <= 1);
static const double gridLimit {slowFactor / sqrt(2) * MotorSystem::stepsPerSecond};

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

}

GridSpeed getTrajectory(Position start, Position end) {
    // Might be abuse of notation unless you consider Position as a vector
    const Position delta {end - start};
    // Scale so that the bigger component moves at max grid speed.
    const double scaleFactor {gridLimit / max(delta.x, delta.y)};
    return GridSpeed{scaleFactor * delta.x, scaleFactor * delta.y};
}

size_t StringState::printTo(Print& p) const {
    return this->toTangential().printTo(p);
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
    speed = GridSpeed(gridLimit, gridLimit);
    // speed = getTrajectory(this->toPosition(), _position);
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
