#include "Scheduler.h"
#include "Time.h"
#include "Parser.h"
namespace Scheduler{

namespace {
Time interval {Time::fromMinutes(0.02)};
Time target {Time(0)};

namespace Tasks {
    int clock = 0;  // For cycles

    Parser::Command printTime(Time time) {
        Serial.print(time.unixTime);
        Serial.print(": ");
        Serial.println(time);
        return Parser::empty;
    }

    Parser::Command makeSquare() {
        constexpr int steps = 4;
        auto command = Parser::empty;
        command.type = Parser::CommandType::Go;
        constexpr double length = 30;

        switch (clock) {
            case 0: command.num1 = length; break;
            case 1: command.num1 = length; command.num2 = length; break;
            case 2: command.num2 = length; break;
            case 3: break;
        }

        clock = (clock + 1) % 4;
        return command;
    }
}
}

void setTime(const String timeString) {
    Time::setTime(timeString);
    target = Time::getNow();
}

void setZone(double offset) {
    Time::setZone(offset);
}

void init(const String timeString, double rate=1) {
    setTime(timeString);
    Time::setSpeed(rate);
}

void setInterval(double minutes) {
    interval = Time::fromMinutes(minutes);
}

// Keeps pace until we reset with fetch()
void run() {
    Time::update();
    ready = target <= Time::getNow();
}

Parser::Command fetch() {
    /**
     * @brief [CHANGED] I considered whether to use the current
     * time or the target time. I decided to keep it on
     * the regular track of target += interval rather
     * than having it be more "faithful" but irregular
     * 
     * Actually, so the system doesn't fall behind, we start 
     * the interval from when we fetch.
     */
    const auto now = Time::getNow();
    // const auto currentCommand = Tasks::printTime(now);
    const auto currentCommand = Tasks::makeSquare();
    if (ready) {
        target = now + interval;
        return currentCommand;
    } else {
        return Parser::empty;
    }
}

bool ready {false};
}
