#include "TimeTest.h"

#include "Time.h"
#include <Arduino.h>

void TimeTest::runAllTests() {
    testUnix();
    testUTC();
    testTimeZone();
    testParsingUTC();
    testParsingTimeZone();
}

void TimeTest::testUnix() {
    auto time = Time{1675194710UL};

    printTime(time, "Test Unix");
}

void TimeTest::testUTC() {
    // 18:38 UTC, or 13:38 ET
    auto time = Time{2023, 01, 31, 18, 38};

    printTime(time, "Test UTC");
}

void TimeTest::testTimeZone() {
    // 13:38 ET, or 18:38 UTC
    auto time = Time{2023, 01, 31, 13, 38, -5};

    printTime(time, "Test TimeZone");
}

void TimeTest::testParsingUTC() {
    auto time = Time{"2023.01.31 18:38"};

    printTime(time, "Test parsing UTC");
}

void TimeTest::testParsingTimeZone() {
    auto time = Time{"2023.01.31 13:38 -5"};

    printTime(time, "Test parsing ET");
}

void TimeTest::printTime(Time time, String description) {
    Serial.println(description);
    Serial.println(time);
    Serial.println(time.unixTime);
    Serial.println();
}