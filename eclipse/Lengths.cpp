#include "Lengths.h"

#include <Arduino.h>

namespace Lengths {

size_t printToPair(Print& p, double first, double second) {
    size_t size = 0;
    
    size += p.print("(");
    size += p.print(first);
    size += p.print(", ");
    size += p.print(second);
    size += p.print(")");

    return size;
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