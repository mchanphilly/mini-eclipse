#include <iostream>
// #include <iomanip>
#include <chrono>
// #include <format>
// export module AngleModel;

using std::ostream;
using std::cout;
namespace krn = std::chrono;
// using namespace std::chrono_literals;

// using std::chrono;

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

        auto operator+(Angle other) -> Angle {
            return Angle(value + other.value);
        };
        
        auto operator-(Angle other) -> Angle {
            return Angle(value - other.value);
        };

        auto friend operator<<(ostream& os, Angle angle) -> ostream& {
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

        auto operator+(AngularCoordinates other) -> AngularCoordinates {
            return AngularCoordinates { angles[0] + other.angles[0], angles[1] + other.angles[1]};
        };

        auto operator-(AngularCoordinates other) -> AngularCoordinates {
            return AngularCoordinates { angles[0] - other.angles[0], angles[1] - other.angles[1]};
        };

        auto friend operator<<(ostream& os, AngularCoordinates coordinates) -> ostream& {
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
    // std::cout << "Test\n";
    // Angle angle = Angle(90);
    // Angle second = Angle(60);
    // Angle sum = angle + second;
    // std::cout << sum;

    auto first = SunAngles(20., 20.);
    auto second = SunAngles(50., 40.);
    std::cout << first - second << "\n";

    cout << "Yes\n";
    auto duration = std::chrono::system_clock::now();
    // std::time_t now = std::chrono::system_clock::to_time_t(clock);

    // std::chrono::duration<double> time = clock;
    // std::cout << clock << std::endl;
    // std::cout << now << std::endl;
    // cout << time << "UTC\n";

    // std::cout << std::chrono::minutes(1h).count() << "minutes\n";
    // std::cout << std::chrono::microseconds(1s).count() << " microseconds\n";
    // std::cout << std::chrono::utc_clock::now() << "\n";
    // cout << duration << "\n";

    // Thanks https://akrzemi1.wordpress.com/2022/04/11/using-stdchrono/
    // auto now = krn::utc_clock::now();
    // std::cout << std::format("Hello {}!\n", "world");
    // cout << std::format("{:%Y-%m-%d %H:%M}\n", now);
    cout << "Ahh\n";
};
