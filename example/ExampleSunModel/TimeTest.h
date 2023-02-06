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

    static void testBlockingComparison();

    // static void testBlockingRealTime();

    static void testBlockingFaster();

    static void testFromMinutes();

    static void testAdd();

private:
    static void printTime(Time time, String description);

    // Update the Time class for seconds, blocking.
    static void blockingUpdate(double seconds);

    static void assertEquals(Time expected, Time actual, String name);
};

#endif