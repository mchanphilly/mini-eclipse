#include "Parser.h"

namespace Parser {

namespace {
    CommandType processType(String commandString) {
        #define parseCase(str, ct) if (commandString.equals(str)) {return CommandType::ct;}

        parseCase("getstep", GetStep);
        parseCase("gostep", GoStep);
        parseCase("step", ShiftStep);

        parseCase("getinch", GetInch);
        parseCase("goinch", GoInch);
        parseCase("inch", ShiftInch);

        parseCase("getpos", GetPosition);
        parseCase("go", Go);
        parseCase("shift", Shift);

        parseCase("fix", SoftZero);
        parseCase("origin", HardZero);
        #undef parseCase

        // Shouldn't get here if we had a valid command.
        return CommandType::Invalid;
    }
}

Command::Command(CommandType t, double n1, double n2)
    :   type{t},
        num1{n1},
        num2{n2}
    {}


size_t Command::printTo(Print& p) const {
    size_t size = 0;
    size += p.print("Command type: ");
    size += p.print((int)type);
    size += p.print(" (");
    size += p.print(num1);
    size += p.print(", ");
    size += p.print(num2);
    size += p.print(")");
    return size;
}


Command parse(String string) {
    String intString1 = "";
    String intString2 = "";

    Serial.println("Entered: " + string);

    int split1 = string.indexOf(" ");
    int split2 = string.indexOf(" ", split1 + 1);

    String commandString = string.substring(0, split1);

    if (split1 > 0) {
        if (split2 > 0) {
            intString1 = string.substring(split1 + 1, split2);
            intString2 = string.substring(split2 + 1);
        } else {
            intString1 = string.substring(split1 + 1);
        }
    }

    CommandType type = processType(commandString);
    double number1 = intString1.toDouble();
    double number2 = intString2.toDouble();

    return Command(type, number1, number2);
}
}