#include "Time.h"

#include <Arduino.h>
#include <TimeLib.h>

Time::Time(time_t unix):
    unixTime{unix}
{}

Time::Time(int year, int month, int day, int hour, int minute, int utcOffset=0):
    unixTime{listToUnix(year, month, day, hour, minute, utcOffset)}
{}

// Makes a Time using a string of form yyyy.mm.dd hh:mm -5
// TODO make work
Time::Time(String timeString):
    unixTime{stringToUnix(timeString)}
{}

size_t Time::printTo(Print &p) const {
    size_t size = 0;
    TimeElements elements;
    breakTime(unixTime, elements);

    #define printadd(e) size += p.print(e)
    printadd(elements.Year + 1970);
    printadd('.');
    printadd(elements.Month);
    printadd('.');
    printadd(elements.Day);
    printadd(' ');
    printadd(elements.Hour);
    printadd(':');
    printadd(elements.Minute);
    printadd(" UTC");
    #undef printadd

    return size;
}

const time_t Time::listToUnix(int year, int month, int day, int hour, int minute, int utcOffset=0) {
    auto elements = TimeElements{0, minute, hour, 0, day, month, year - 1970};
    time_t outTime = makeTime(elements);

    // Timezone adjustment
    long offsetSeconds = utcOffset * SECS_PER_HOUR;

    return outTime - offsetSeconds;  // minus because that's how it works in timezones
}

// TODO make work
const time_t Time::stringToUnix(const String timeString) {
    constexpr int numFields {6};  // Year, Month, Day,  Hour, Minute, UTCOffset
    constexpr int numDelimiters {numFields - 1};

    // Start with our known delimiters of the fields and figure the boundaries.
    constexpr char delimiters[] {'.', '.', ' ', ':', ' '};
    int delimiterIndex[numDelimiters];

    delimiterIndex[0] = timeString.indexOf(delimiters[0]);

    for (int i = 1; i < numFields; i++) {
        delimiterIndex[i] = timeString.indexOf(delimiters[i], delimiterIndex[i - 1] + 1);
    }

    // Figure the substrings afterward.
    int fields[numFields];

    // Year
    fields[0] = timeString.substring(0, delimiterIndex[0]).toInt();

    // All else but time zone
    for (int i = 1; i < numDelimiters; i++) {
        fields[i] = timeString.substring(delimiterIndex[i - 1] + 1, delimiterIndex[i]).toInt();
    }

    // Check for time zone
    int lastIndex = delimiterIndex[numDelimiters - 1];
    fields[numDelimiters] = lastIndex == -1 ? 0 : timeString.substring(lastIndex).toInt();

    return listToUnix(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
}