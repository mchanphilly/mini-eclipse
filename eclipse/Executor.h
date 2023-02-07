#ifndef Executor_h
#define Executor_h

#include "Blocker.h"
#include "Parser.h"

namespace Executor {
    void init();

    void run();

    void execute(Parser::Command command);

    const Blocker::StringState& getState();
}

#endif