#include "SunModel.h"

#include <Arduino.h>
#include <SolarCalculator.h>
#include "Time.h"

void SunModel::Location::print() const {
    printPair("Location", latitude, longitude);
}

SunModel::SolarAngles SunModel::SolarAngles::operator-(const SolarAngles& other) const {
    auto _azimuth = this->azimuth - other.azimuth;
    auto _altitude = this->altitude - other.altitude;
    return SolarAngles{_azimuth, _altitude};
}

void SunModel::SolarAngles::print() const {
    printPair("SolarAngles", azimuth, altitude);
}

SunModel::SunModel(
        const double latitude,
        const double longitude,
        const double windowAzimuth,
        const double windowAltitude):
            location{latitude, longitude},
            windowOffset{windowAzimuth, windowAltitude}
    {}

SunModel::SunModel(const Location _location, const SolarAngles _windowOffset):
        location(_location),
        windowOffset(_windowOffset)
    {}

SunModel::SolarAngles SunModel::anglesAt(Time time) const {
        auto raw = rawAngles(time);
        auto adjusted = adjustForWindow(raw);
        return adjusted;
}

SunModel::SolarAngles SunModel::rawAngles(Time time) const {
        double azimuth, altitude;
        calcHorizontalCoordinates(
            time.unixTime,
            location.latitude,
            location.longitude,
            azimuth,
            altitude);

        return SolarAngles{azimuth, altitude};
}

SunModel::SolarAngles SunModel::adjustForWindow(SolarAngles raw) const {
        return raw - windowOffset;
}

void printPair(String string, double first, double second) {
    // Why not printf? Because doubles (and floats) aren't supported.
    Serial.print(string);
    Serial.print(": (");
    Serial.print(first);
    Serial.print(", ");
    Serial.print(second);
    Serial.println(")");
}