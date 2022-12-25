class GlobalCoordinates {
    float latitude;
    float longitude;
};

class SunAngles {
    float azimuth;
    float altitude;

    public:
    
    const SunAngles adjust(const SunAngles offset) {
        const float outAzimuth = azimuth + offset.azimuth;
        const float outAltitude = altitude + offset.altitude;
    };
};


class AngleModel {
    const GlobalCoordinates location;
    const SunAngles windowOffset;

    SunAngles calculateSolarCoordinates(float time) {
        // Using the approximate algorithms given on https://gml.noaa.gov/grad/solcalc/calcdetails.html
        // TODO
    };

    SunAngles calculateWindowCoordinates(float time) {
        SunAngles northAngles = calculateSolarCoordinates(time);

        const SunAngles windowAngles = northAngles.adjust(windowOffset);
    }
};