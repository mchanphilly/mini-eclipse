// #include <TimeLib.h>
// #include "MyTime.h"
// #include "SunModel.h"
#include "MyTimeTest.h"

// const auto home = SunModel::Location{42.36002, -71.08788};
// const auto homeOffset = SunModel::SolarAngles{155.75, 0};
// auto model = SunModel(home, homeOffset);


void setup() {
    Serial.begin(9600);
    Serial.println();

    MyTimeTest::testUnix();

    MyTimeTest::testUTC();

    MyTimeTest::testTimeZone();

    MyTimeTest::testParsingUTC();

    MyTimeTest::testParsingTimeZone();
}

void loop() {
    
}