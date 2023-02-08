#ifndef Blocker_h
#define Blocker_h

#include <Arduino.h>
#include "Lengths.h"
#include "MotorSystem.h"

/**
 * @brief Responsible for tracking the state of the physical system.
 * 
 */
namespace Blocker {
using namespace Lengths;

class StringState : public Printable {
public:
    StringState() = default;

    StringState(Radial _radial);

    StringState(Tangential _tangential);

    // No StringState(Position) because insufficient information (origin?)

    void go(Position _position);

    void softZero(Tangential tangential);

    void hardZero(Tangential tangential);

    StringSpeed getSpeed() const;

    Radial toRadial() const;

    Tangential toTangential() const;

    Position toPosition() const;  // Uses declared origin as origin

    TotalLengths toTotalLengths() const;

    size_t printTo(Print& p) const;

private:
    TruePosition toTruePosition() const;  // Uses center of left spool as origin

    // Choosing radial for the internal representation.
    Radial radial{0, 0};  // For current configuration
    GridSpeed speed{0, 0};  // For current trajectory; changes with 'go' call
    double originOffset{10};  // vertical offset from declared origin
};

}

#endif
