#ifndef Time_h
#define Time_h

#include <Arduino.h>
#include <TimeLib.h>

/**
 * @brief Might not be worth including in a class, but
 * it's basically just to parse timeStrings and to print.
 * 
 * Mostly a wrapper class.
 * 
 */
class Time : public Printable {
public:
    Time(time_t unix);

    Time(int year, int month, int day, int hour, int minute, int utcOffset=0);
    
    Time(const String timeString);

    size_t printTo(Print& p) const;

    // void increment(int hour, int minute);
    static void setTime(Time time);
    static void setTime(const String timeString);

    // Accelerate the rate at which new time passes by a multiplier
    static void setSpeed(double multiplier);

    static void update();

    static Time getNow();
    static time_t getRawNow();

    static Time fromMinutes(double minutes);

    bool operator==(const Time& other) const;
    bool operator<(const Time& other) const;
    
    Time operator+(const Time& other) const;

    // todo check if this can be private
    static double rate;

    time_t unixTime;
private:
    time_t listToUnix(int year, int month, int day, int hour, int minute, int utcOffset=0);
    time_t stringToUnix(String timeString);

    inline static time_t seconds() {
        return millis() / 1000;
    }

    static time_t now;
    static time_t lastPolled;
};

#endif