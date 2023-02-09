#ifndef Executor_h
#define Executor_h

#include "Lengths.h"
#include "Parser.h"

namespace Executor {

void init(Lengths::Tangential tangential);

void run();

void execute(Parser::Command command);
}

#endif