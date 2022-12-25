#include <iostream>
// export module AngleModel;

using std::ostream;

/**
 * @brief Represents an angle, by default in degrees.
 * 
 */
class Angle {
    // Generally stored in degrees between some minimum and max angle.
    // By convention, we keep it between 0 and 360 degrees.
    public:
        Angle(double v)
            : value(v)
        {}

        Angle operator+(Angle other) {
            return Angle(value + other.value);
        };
        
        Angle operator-(Angle other) {
            return Angle(value - other.value);
        };

        friend ostream& operator<<(ostream& os, Angle angle) {
            os << angle.value;
            return os;
        };
            
    private:
        double value;
};

/**
 * @brief Represents an angular coordinate pair.
 * 
 */
class AngularCoordinates {
    public:
        AngularCoordinates():
            angles(0, 0)
        {}

        AngularCoordinates(double a, double b):
            angles(Angle(a), Angle(b))
        {}

        AngularCoordinates(Angle a, Angle b):
            angles(a, b)
        {}

        AngularCoordinates operator+(AngularCoordinates other) {
            return AngularCoordinates { angles[0] + other.angles[0], angles[1] + other.angles[1]};
        };

        AngularCoordinates operator-(AngularCoordinates other) {
            return AngularCoordinates { angles[0] - other.angles[0], angles[1] - other.angles[1]};
        };

        friend ostream& operator<<(ostream& os, AngularCoordinates coordinates) {
            os << "(" << coordinates.angles[0] << ", " << coordinates.angles[1] << ")";
            return os;
        };


    private:
        Angle angles[2];
};

/**
 * @brief Represents a latitude-longitude pair on the planet.
 * 
 */
class Location : public AngularCoordinates {
    public:
        Location():
            AngularCoordinates{ 42.360015, -71.087902 }
        {}

        Location(double a, double b):
            AngularCoordinates(a, b)
        {}
    private:
};

/**
 * @brief Represents an azimuth-altitude pair of the sun for calculations.
 * 
 */
class SunAngles : public AngularCoordinates {
    public:
        SunAngles(double a, double b):
            AngularCoordinates(a, b)
        {}

    private:
};

/**
 * @brief Represents the sun's angle when viewed at a location and time.
 * 
 */
class SunPosition {
    public:
        SunPosition(Location loc, SunAngles offset):
            location(loc), windowOffset(offset)
        {}
        
        // SunAngles calculateSolarCoordinates(float time) {
        //     // Using the approximate algorithms given on https://gml.noaa.gov/grad/solcalc/calcdetails.html
        //     // TODO
        //     return SunAngles(0, 0);
        // };

        // SunAngles calculateWindowCoordinates(float time) {
        //     SunAngles northAngles = calculateSolarCoordinates(time);
        //     // const SunAngles windowAngles = northAngles.adjust(windowOffset);
        //     return northAngles;
        // };


    private:
        Location location;
        SunAngles windowOffset;
};

int main() {
    std::cout << "Test\n";
    // Angle angle = Angle(90);
    // Angle second = Angle(60);
    // Angle sum = angle + second;
    // std::cout << sum;

    auto first = SunAngles(20., 20.);
    auto second = SunAngles(50., 40.);
    std::cout << first - second;
};
