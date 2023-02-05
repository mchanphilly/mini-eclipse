#ifndef Executor_h
#define Executor_h

#include "BlockerSystem.h"
#include "Parser.h"

class Executor {
public:
    void init();

    void run();

    void execute(Parser::Command command);

    const BlockerSystem::StringState& getState() const;
private:
    inline void printPosition();

    void printTangential();

    void go(BlockerSystem::Position position);

    void zero(BlockerSystem::Tangential tangential);

    BlockerSystem blocker;
    MotorSystem motors;
};

#endif