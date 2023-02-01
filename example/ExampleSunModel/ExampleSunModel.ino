#include "MyTimeTest.h"
#include "SunModelTest.h"


void setup() {
    Serial.begin(9600);
    Serial.println();

    // MyTimeTest::runAllTests();
    SunModelTest::runAllTests();
}

void loop() {
    
}