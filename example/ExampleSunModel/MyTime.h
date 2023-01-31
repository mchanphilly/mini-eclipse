#ifndef MyTime_h
#define MyTime_h

#include <TimeLib.h>

class MyTime : public Printable {
public:
    MyTime(int year, int month, int day, int hour, int minute, int utcOffset=0);
    
    MyTime(String timeString);
    // Just learned about const functions. Huh.
    // Prints to the Serial a human readable version of the time.
    size_t printTo(Print& p) const;
    
    // void increment(int hour, int minute);

    const time_t unixTime;
private:
    time_t listToUnix(int year, int month, int day, int hour, int minute, int utcOffset=0) const;
};

#endif