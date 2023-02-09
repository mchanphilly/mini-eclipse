#include "Lengths.h"

#include <Arduino.h>

namespace Lengths {
namespace {
size_t printToPair(Print& p, double first, double second) {
    size_t size = 0;
    
    size += p.print("(");
    size += p.print(first);
    size += p.print(", ");
    size += p.print(second);
    size += p.print(")");

    return size;
}

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

// todo refactor
Radial::Radial(Tangential tangential)
    : Radial(getHypotenuses<Radial>(tangential.left, tangential.right, radius))
    {}

Tangential::Tangential(Radial radial)
    : Tangential(getLegs<Tangential>(radial.left, radial.right, radius))
    {}

Steps::Steps(long _left, long _right)
    : left{_left}, right{_right}
    {} 

Steps::Steps(double pair[2])
    : left{lround(pair[0])}, right{lround(pair[1])}
    {}

size_t GridPair::printTo(Print& p) const {
    return p.print("GridPair: ") + printToPair(p, this->x, this->y);
}

size_t StringPair::printTo(Print& p) const {
    return p.print("StringPair: ") + printToPair(p, this->left, this->right);
}

size_t Steps::printTo(Print& p) const {
    return p.print("Steps: ") + printToPair(p, this->left, this->right);
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

}