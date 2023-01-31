#include "MyTime.h"

#include <Arduino.h>
#include <TimeLib.h>

MyTime::MyTime(int year, int month, int day, int hour, int minute, int utcOffset=0):
    unixTime{listToUnix(year, month, day, hour, minute, utcOffset)}
{}

// Makes a MyTime using a string of form yyyy.mm.dd hh:mm -5
MyTime::MyTime(String timeString):
    unixTime{stringToUnix(timeString)}
{}

size_t MyTime::printTo(Print &p) const {
    size_t size = 0;
    TimeElements elements;
    breakTime(unixTime, elements);

    #define printadd(e) size += p.print(e)
    printadd(elements.Year);
    printadd('.');
    printadd(elements.Month);
    printadd('.');
    printadd(elements.Day);
    printadd(' ');
    printadd(elements.Hour);
    printadd(':');
    printadd(elements.Minute);
    #undef printadd

    return size;
}

const time_t MyTime::listToUnix(int year, int month, int day, int hour, int minute, int utcOffset=0) {
    auto elements = TimeElements{0, minute, hour, 0, day, month, year - 1970};
    size_t outTime = makeTime(elements);

    // Timezone adjustment
    long offsetSeconds = utcOffset * SECS_PER_HOUR;
    Serial.println("Offset seconds may be negative");
    Serial.println(offsetSeconds);

    return outTime + offsetSeconds;
}

const time_t MyTime::stringToUnix(String timeString) {
    constexpr int numFields {6};
    constexpr int numDelimiters {numFields - 1};

    // Start with our known delimiters of the fields
    constexpr char delimiters[] {'.', '.', ' ', ':', ' '};
    int delimiterIndex[numDelimiters];

    delimiterIndex[0] = timeString.indexOf(delimiters[0]);
    for (int i = 1; i < numFields; i++) {
        delimiterIndex[i] = timeString.indexOf(delimiters[i], delimiterIndex[i - 1] + 1);
    }

    // Get the fields between the delimiters
    int fields[numFields];
    fields[0] = timeString.substring(0, delimiterIndex[0]).toInt();

    for (int i = 1; i < numDelimiters; i++) {
        fields[i] = timeString.substring(delimiterIndex[i - 1] + 1, delimiterIndex[i]).toInt();
    }

    // Last one is optional (timezone), so we put in a little more work.
    fields[numDelimiters - 1] = delimiterIndex[4] == -1 ? 0 : timeString.substring(delimiterIndex[4] + 1).toInt();

    return listToUnix(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
}