#ifndef MyTimeTest_h
#define MyTimeTest_h

#include <Arduino.h>
#include "MyTime.h"

// Test the different member functions of MyTime
class MyTimeTest {
public:
    static void runAllTests();

    static void testUnix();

    static void testUTC();

    static void testTimeZone();
    
    static void testParsingUTC();

    static void testParsingTimeZone();

private:
    static void printTime(MyTime time, String description);
};

#endif