#include "Time.h"

#include <Arduino.h>
#include <TimeLib.h>

double Time::rate {1};
time_t Time::now {0};
time_t Time::lastPolled {0};
double Time::utcOffset {-5};  // for ET

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
    
    auto adjusted = fromUTC(unixTime, utcOffset);

    breakTime(adjusted, elements);

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

void Time::setSpeed(double multiplier) {
    if (multiplier < 0) {
        Serial.println("Bad rate");
    }
    
    rate = multiplier;
}

void Time::setTime(Time time) {
    Time::lastPolled = seconds();
    Time::now = time.unixTime;
    // Could refactor to use update() if wanted.
}

void Time::setTime(const String timeString) {
    const auto time = Time(timeString);
    Time::setTime(time);
}

void Time::setZone(double offset) {
    utcOffset = offset;
}

// Undefined behavior if now() is called
// at intervals greater than rollover time
void Time::update() {
    // Note that there's a very light drift since
    // we implicitly call millis() two different times.
    auto elapsed = seconds() - Time::lastPolled;
    if (elapsed) {  // Seconds are slower, so let's not risk it if unnecessary.
        Time::lastPolled = seconds();
        Time::now += elapsed * static_cast<unsigned long>(rate);
    }
}

Time Time::getNow() {
    return Time(now);
}

time_t Time::getRawNow() {
    return now;
}

Time Time::fromMinutes(double minutes) {
    return Time(SECS_PER_MIN * minutes);
}

bool Time::operator==(const Time& other) const {
    return this->unixTime == other.unixTime;
}

bool Time::operator<(const Time& other) const {
    return this->unixTime < other.unixTime;
}

bool Time::operator<=(const Time& other) const {
    return this->operator<(other) || this->operator==(other);
}

Time Time::operator+(const Time& other) const {
    return Time(this->unixTime + other.unixTime);
}


time_t Time::listToUnix(int year, int month, int day, int hour, int minute, int offset=0) {
    auto elements = TimeElements{0, minute, hour, 0, day, month, year - 1970};
    time_t outTime = makeTime(elements);

    // Timezone adjustment
    return toUTC(outTime, offset);
}


time_t Time::stringToUnix(const String timeString) {
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
