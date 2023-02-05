#include "TimeTest.h"

#include "Time.h"
#include <Arduino.h>
// #include <assert.h>

// "2023.01.31 18:38 UTC"
constexpr auto refUnix = 1675190280UL;

void TimeTest::runAllTests() {
    Serial.println("Run all Time tests");
    testUnix();
    testUTC();
    testTimeZone();
    testParsingUTC();
    testParsingTimeZone();
    testBlockingComparison();
    // testBlockingRealTime();
    testBlockingFaster();
    Serial.println("All Time tests passed");
}

void TimeTest::testUnix() {
    auto time = Time{refUnix};
    assertEquals(refUnix, time.unixTime, "Test Unix");
    // printTime(time, "Test Unix");
}

void TimeTest::testUTC() {
    // 18:38 UTC, or 13:38 ET
    auto time = Time{2023, 01, 31, 18, 38};
    assertEquals(refUnix, time.unixTime, "Test UTC");
    // Serial.println(refUnix);
    // Serial.println(time.unixTime);
    // Serial.println("UTC pass");
}

void TimeTest::testTimeZone() {
    // 13:38 ET, or 18:38 UTC
    auto time = Time{2023, 01, 31, 13, 38, -5};
    assertEquals(refUnix, time.unixTime, "Test TimeZone");
    // printTime(time, "Test TimeZone");
    // Serial.println("Time Zone pass");
}

void TimeTest::testParsingUTC() {
    auto time = Time{"2023.01.31 18:38"};
    assertEquals(refUnix, time.unixTime, "Test Parse UTC");
    // Serial.println("Parse UTC pass");
    // printTime(time, "Test parsing UTC");
}

void TimeTest::testParsingTimeZone() {
    auto time = Time{"2023.01.31 13:38 -5"};
    assertEquals(refUnix, time.unixTime, "Test Parse TimeZone");
    // Serial.println("Parse time zone pass");
    // printTime(time, "Test parsing ET");
}

void TimeTest::testBlockingComparison() {
    auto earlier = Time("2023.01.31 13:38 -5");
    auto later = Time("2023.01.31 14:38 -5");
    // assert(earlier < later);
    Serial.println("Should be true");
    Serial.println(earlier < later);
    // Serial.println("Blocking comparison pass");
}


// void TimeTest::testBlockingRealTime() {

// }

void TimeTest::testBlockingFaster() {
    Time::setSpeed(60);
    Time::setTime(Time(refUnix));

    Serial.println(Time::getNow());
    blockingUpdate(5);
    Serial.println(Time::getNow());
}

void TimeTest::printTime(Time time, String description) {
    Serial.println(description);
    Serial.println(time);
    Serial.println(time.unixTime);
    Serial.println();
}

void TimeTest::blockingUpdate(double seconds) {
    // assert(seconds > 0);
    auto millisNow = millis();
    const unsigned long testMillis = seconds * 1000UL;
    while (millis() - millisNow < testMillis) {
        Time::update();
    }
}

void TimeTest::assertEquals(Time expected, Time actual, String name) {
    // assert(expected == actual);
    Serial.print(name);
    Serial.print(": ");
    if (expected == actual) {
        Serial.println("Pass");
    } else {
        Serial.println("Fail: ");
        Serial.print(expected);
        Serial.print(" expected but ");
        Serial.print(actual);
        Serial.println(" received");
    }
}