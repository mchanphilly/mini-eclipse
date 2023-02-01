#include "MyTimeTest.h"

#include "MyTime.h"
#include <Arduino.h>

void MyTimeTest::testUnix() {
    auto time = MyTime{1675194710UL};

    printTime(time, "Test Unix");
}

void MyTimeTest::testUTC() {
    // 18:38 UTC, or 13:38 ET
    auto time = MyTime{2023, 01, 31, 18, 38};

    printTime(time, "Test UTC");
}

void MyTimeTest::testTimeZone() {
    // 13:38 ET, or 18:38 UTC
    auto time = MyTime{2023, 01, 31, 13, 38, -5};

    printTime(time, "Test TimeZone");
}

void MyTimeTest::testParsingUTC() {
    auto time = MyTime{"2023.01.31 18:38"};

    printTime(time, "Test parsing UTC");
}

void MyTimeTest::testParsingTimeZone() {
    auto time = MyTime{"2023.01.31 13:38 -5"};

    printTime(time, "Test parsing ET");
}

void MyTimeTest::printTime(MyTime time, String description) {
    Serial.println(description);
    Serial.println(time);
    Serial.println(time.unixTime);
    Serial.println();
}