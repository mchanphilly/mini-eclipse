#ifndef Executor_h
#define Executor_h

#include "BlockerSystem.h"
#include "Parser.h"

namespace Executor {
    void init();

    void run();

    void execute(Parser::Command command);

    const BlockerSystem::StringState& getState();
}

#endif