#include "TimeTest.h"
#include "SunModelTest.h"


void setup() {
    Serial.begin(9600);
    Serial.println();

    TimeTest::runAllTests();
    SunModelTest::runAllTests();
}

void loop() {
    
}