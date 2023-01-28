#ifndef SunModel_h
#define SunModel_h

#include <SolarCalculator.h>
#include <TimeLib.h>


class SunModel {
    static void printPair(String string, double first, double second) {
        Serial.print(string);
        Serial.print(": (");
        Serial.print(first);
        Serial.print(", ");
        Serial.print(second);
        Serial.println(")");
    }

    public:
    struct Location {
        double latitude;
        double longitude;

        void print() {
            printPair("Location", latitude, longitude);
        }
    };

    struct SolarAngles {
        double azimuth;
        double altitude;

        SolarAngles operator-(const SolarAngles& other) {
            SolarAngles out;
            out.azimuth = this->azimuth - other.azimuth;
            out.altitude = this->altitude - other.altitude;
            return out;
        }

        void print() {
            printPair("SolarAngles", azimuth, altitude);
        }
    };

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
    SolarAngles rawAngles(unsigned long time) {
        double azimuth, altitude;
        calcHorizontalCoordinates(
            time,
            location.latitude,
            location.longitude,
            azimuth,
            altitude);

        return SolarAngles{azimuth, altitude};
    }

    SolarAngles adjustForWindow(SolarAngles raw) {
        return raw - windowOffset;
    }

    public:

    SunModel(
        const double latitude,
        const double longitude,
        const double windowAzimuth,
        const double windowAltitude):
            location{latitude, longitude},
            windowOffset{windowAzimuth, windowAltitude}
    {}

    /**
     * @brief Return the angles associated with the time.
     * 
     * @param time 
     * @return SolarAngles 
     */
    SolarAngles getAngles(unsigned long time) {
        auto raw = rawAngles(time);
        auto adjusted = adjustForWindow(raw);
        return adjusted;
    }
};

#endif