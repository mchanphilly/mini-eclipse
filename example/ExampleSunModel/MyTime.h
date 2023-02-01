#ifndef MyTime_h
#define MyTime_h

#include <Arduino.h>
#include <TimeLib.h>

class MyTime : public Printable {
public:
    MyTime(unsigned long unix);

    MyTime(int year, int month, int day, int hour, int minute, int utcOffset=0);
    
    MyTime(const String timeString);
    // Just learned about const functions. Huh.
    // Prints to the Serial a human readable version of the time.
    size_t printTo(Print& p) const;
    
    // void increment(int hour, int minute);

    const time_t unixTime;
private:
    const time_t listToUnix(int year, int month, int day, int hour, int minute, int utcOffset=0);
    const time_t stringToUnix(String timeString);
};

#endif