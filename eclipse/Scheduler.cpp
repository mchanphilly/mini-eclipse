#include "Scheduler.h"
#include "Time.h"
#include "Parser.h"
namespace Scheduler{

namespace {
Time interval {Time::fromMinutes(0.02)};
Time target {Time(0)};

namespace Tasks {
    Parser::Command printTime(Time time) {
        Serial.println(time.unixTime);
        return Parser::empty;
    }
}
}

void init(const String timeString, double rate=1) {
    Time::setTime(timeString);
    Time::setSpeed(rate);
    target = Time::getNow();
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
    const auto currentCommand = Tasks::printTime(now);
    if (ready) {
        target = now + interval;
        return currentCommand;
    } else {
        return Parser::empty;
    }
}

bool ready {false};
}
