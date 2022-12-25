#include <iostream>
// export module AngleModel;


class Angle {
    // Generally stored in degrees between some minimum and max angle.
    // By convention, we keep it between 0 and 360 degrees.
    public:
        Angle(double v)
            : value(v)
        {}
            
    private:
        double value;
};


class AngularCoordinates {
    public:
        AngularCoordinates(double lat, double lon)
            : latitude(Angle(lat)),
              longitude(Angle(lon))
        {}

        AngularCoordinates(Angle lat, Angle lon)
            : latitude(lat),
              longitude(lon)
        {}

    private:
        Angle latitude;
        Angle longitude;
};

class SunAngles {
    // Coordinates for the original installation site
    AngularCoordinates home =
        AngularCoordinates(42.360015, -71.087902);

    // public:
    

    // // const SunAngles adjust(const SunAngles offset) {
    // //     const float outAzimuth = azimuth + offset.azimuth;
    // //     const float outAltitude = altitude + offset.altitude;
    // // };

    // private:
    //     AngularCoordinates location;
};


class AngleModel {
        // AngularCoordinates location;
        // SunAngles windowOffset;

    // SunAngles calculateSolarCoordinates(float time) {
    //     // Using the approximate algorithms given on https://gml.noaa.gov/grad/solcalc/calcdetails.html
    //     // TODO
    // };

    // SunAngles calculateWindowCoordinates(float time) {
    //     SunAngles northAngles = calculateSolarCoordinates(time);

    //     const SunAngles windowAngles = northAngles.adjust(windowOffset);
    // };
};

int main() {
    std::cout << "Test\n";
};
