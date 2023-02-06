#include "Scheduler.h"
#include "Time.h"
#include "Parser.h"
namespace Scheduler{
namespace {
Parser::Command perform(Time time) {
    // Filler for now
    const auto command = Parser::Command(
        Parser::CommandType::Invalid,
        (double)time.unixTime,
        0
    );
    return command;
}

Time interval {Time::fromMinutes(0.02)};
Time target {Time(0)};
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
    ready = target < Time::getNow();
}

Parser::Command fetch() {
    /**
     * @brief I considered whether to use the current
     * time or the target time. I decided to keep it on
     * the regular track of target += interval rather
     * than having it be more "faithful" but irregular
     */
    const auto currentCommand = perform(target);
    if (ready) {
        target = target + interval;
        return currentCommand;
    } else {
        return Parser::empty;
    }
}

bool ready {false};
}
