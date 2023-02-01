#ifndef Time_h
#define Time_h

#include <Arduino.h>
#include <TimeLib.h>

class Time : public Printable {
public:
    Time(unsigned long unix);

    Time(int year, int month, int day, int hour, int minute, int utcOffset=0);
    
    Time(const String timeString);
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