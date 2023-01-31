#ifndef SunModel_h
#define SunModel_h

#include <Arduino.h>

class SunModel {
public:
    struct Location {
        const double latitude;
        const double longitude;

        void print() const;
    };

    struct SolarAngles {
        const double azimuth;
        const double altitude;

        SolarAngles operator-(const SolarAngles& other) const;

        void print() const;
    };

    SunModel(
        const double latitude,
        const double longitude,
        const double windowAzimuth,
        const double windowAltitude);

    SunModel(const Location _location, const SolarAngles _windowOffset);

    /**
     * @brief Return the angles associated with the time.
     * 
     * @param time 
     * @return SolarAngles 
     */
    SolarAngles anglesAt(unsigned long time) const;

private:
    const Location location;

    // Solar angles corresponding to the perpendicular to the window.
    const SolarAngles windowOffset;

    /**
     * @brief Return the raw angles associated with the time at the location.
     * 
     * @param time 
     * @return SolarAngles 
     */
    SolarAngles rawAngles(unsigned long time) const;

    SolarAngles adjustForWindow(SolarAngles raw) const;
};

/**
 * @brief Debug function to print a pair of doubles.
 * 
 * @param string 
 * @param first 
 * @param second 
 */
void printPair(String string, double first, double second);

#endif