#ifndef SunModelTest_h
#define SunModelTest_h

#include <Arduino.h>
#include "SunModel.h"

class SunModelTest {
public:
    static void runAllTests();

    static void testLocation();

    static void testSolarAngles();

    static void testSubtractSolarAngles();

    static void testLongConstructor();

    static void testStructConstructor();

    static void testAnglesAtThreeTimes();

private:
    static void testAngles(const SunModel &model, const String timeString);
};

#endif