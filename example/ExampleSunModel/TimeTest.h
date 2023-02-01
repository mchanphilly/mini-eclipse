#ifndef TimeTest_h
#define TimeTest_h

#include <Arduino.h>
#include "Time.h"

// Test the different member functions of Time
class TimeTest {
public:
    static void runAllTests();

    static void testUnix();

    static void testUTC();

    static void testTimeZone();
    
    static void testParsingUTC();

    static void testParsingTimeZone();

private:
    static void printTime(Time time, String description);
};

#endif