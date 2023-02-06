#ifndef Scheduler_h
#define Scheduler_h

#include <Arduino.h>
#include "Parser.h"

namespace Scheduler{
    // Initial implementation: simple long non-blocking loop.
    void init(const String timeString, double rate=1);

    void setInterval(double minutes);

    void run();

    Parser::Command fetch();

    extern bool ready;  // extern necessary to avoid linkage error
}

#endif