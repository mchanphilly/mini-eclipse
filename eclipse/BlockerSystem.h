#ifndef BlockerSystem_h
#define BlockerSystem_h

class BlockerSystem {
    const double width;
    const double height;
    const double inchesBelowMotors = 2;

    public:
    BlockerSystem(const double w, const double h):
        width(w), height(h) {};

    /**
     * @brief Convert cartesian (x,y) on window into string lengths
     * 
     * @param x 
     * @param y 
     * @return double* 
     */
    double* toLengths(double x, double y) {
        double lengths[2];

        lengths[0] = sqrt(sq(x) + sq(y + inchesBelowMotors));
        lengths[1] = sqrt(sq(width - x) + sq(y + inchesBelowMotors));

        return lengths;
    }
};

#endif