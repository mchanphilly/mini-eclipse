#include <SolarCalculator.h>
#include <TimeLib.h>
#include "MyTime.h"

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

    /**
     * @brief Construct a new Sun Model object
     * 
     * @param latitude 
     * @param longitude 
     * @param windowAzimuth 
     * @param windowAltitude 
     */
    SunModel(
        const double latitude,
        const double longitude,
        const double windowAzimuth,
        const double windowAltitude):
            location{latitude, longitude},
            windowOffset{windowAzimuth, windowAltitude}
    {}

    SunModel(const Location _location, const SolarAngles _windowOffset):
        location(_location),
        windowOffset(_windowOffset)
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

const auto home = SunModel::Location{42.36002, -71.08788};
const auto homeOffset = SunModel::SolarAngles{155.75, 0};
auto model = SunModel(home, homeOffset);

class SunModelTest {
    public:
        // Note that this is in UTC; should do another thing if we're doing a local time.
        // Probably use adjustment in the API
        TimeElements noonTime = TimeElements{0, 26, 10, 7, 28, 01, 2023 - 1970};

    /**
     * @brief Tests construction of time
     * 
     */
    void test1() {
        // const auto noonTime = makeTime(noonTimeElements);

        auto year = noonTime.Year;
        auto month = noonTime.Month;
        auto day = noonTime.Day;
        auto weekday = noonTime.Wday;
        auto hour = noonTime.Hour;
        auto minute = noonTime.Minute;
        auto second = noonTime.Second;

        const uint8_t units[7] = {
            year, month, day, weekday, hour, minute, second
        };

        char buffer[20];
        sprintf(buffer, "%d.%d.%d %d:%d:%d", year + 1970, month, day, hour, minute, second);
        // const auto noonAngle = SunModel::SolarAngles{25.96, 155.81};
        Serial.println(buffer);
    }

    void test2() {
        auto noonUnix = makeTime(noonTime);
        Serial.println(noonUnix);
    }

    void test3() {

    }
};


void setup() {
    Serial.begin(9600);
    Serial.println();
    SunModelTest testModule;
    testModule.test1();
    testModule.test2();
}

void loop() {
    
}