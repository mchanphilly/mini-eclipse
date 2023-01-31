#include "MyTimeTest.h"

#include "MyTime.h"
#include <Arduino.h>

void MyTimeTest::testConstruction() {
    auto time = MyTime{2023, 01, 31, 13, 38, -5};

    Serial.println("Test construction");
    Serial.println(time);
    Serial.println(time.unixTime);
}

void MyTimeTest::testParsing() {
    auto time = MyTime{"2023.01.31 13:38 -5"};

    Serial.println("Test parsing");
    Serial.println(time);
    Serial.println(time.unixTime);
}