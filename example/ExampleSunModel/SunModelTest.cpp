#include "SunModelTest.h"
#include "SunModel.h"
#include "Time.h"

void SunModelTest::runAllTests() {
    testLocation();
    testSolarAngles();
    testSubtractSolarAngles();

    testLongConstructor();
    testStructConstructor();
    testAnglesAtThreeTimes();

}

constexpr double homeLatitude {42.36002};
constexpr double homeLongitude {-71.08788};
constexpr double windowAzimuth {155.75};
constexpr double windowAltitude {0};

void SunModelTest::testLocation() {
    const auto location {SunModel::Location{homeLatitude, homeLongitude}};
    location.print();
}

void SunModelTest::testSolarAngles() {
    const auto homeOffset {SunModel::SolarAngles{windowAzimuth, windowAltitude}};
    homeOffset.print();
}

void SunModelTest::testSubtractSolarAngles() {
    const auto first {SunModel::SolarAngles{90, 60}};
    const auto second {SunModel::SolarAngles{45, 45}};
    const auto difference = first - second;

    difference.print();
}

void SunModelTest::testLongConstructor() {
    const auto model {SunModel{homeLatitude, homeLongitude, windowAzimuth, windowAltitude}};
}

void SunModelTest::testStructConstructor() {
    const auto location {SunModel::Location{homeLatitude, homeLongitude}};
    const auto homeOffset {SunModel::SolarAngles{windowAzimuth, windowAltitude}};
    const auto model {SunModel{location, homeOffset}};
}

void SunModelTest::testAnglesAtThreeTimes() {
    const auto location {SunModel::Location{homeLatitude, homeLongitude}};
    const auto homeOffset {SunModel::SolarAngles{windowAzimuth, windowAltitude}};
    const auto model {SunModel{location, homeOffset}};

    // Sunrise, middle, sunset
    const String timeStrings[] {
        "2023.02.01 06:56 -5",
        "2023.02.01 10:29 -5",
        "2023.02.01 16:58 -5"
    };

    for (auto &time : timeStrings) {
        testAngles(model, time);
    }
}

void SunModelTest::testAngles(const SunModel &model, const String timeString) {
    auto time {Time{timeString}};
    auto angles {model.anglesAt(time)};

    angles.print();
}