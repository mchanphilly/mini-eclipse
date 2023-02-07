#include "Parser.h"

namespace Parser {

namespace {
    CommandType processType(String commandString) {

        #define parseCase(str, ct) if (commandString.equals(str)) \
            {return CommandType::ct;}

        parseCase("getstep", GetStep);
        parseCase("gostep", GoStep);
        parseCase("step", ShiftStep);

        parseCase("getinch", GetInch);
        parseCase("goinch", GoInch);
        parseCase("inch", ShiftInch);

        parseCase("getpos", GetPosition);
        parseCase("go", Go);

        parseCase("fix", SoftZero);

        parseCase("settime", SetTime);

        #undef parseCase

        return CommandType::Invalid;
    }
}

Command::Command(CommandType t, String _data, double n1, double n2)
    :   type{t},
        data{_data},
        num1{n1},
        num2{n2}
    {}


size_t Command::printTo(Print& p) const {
    size_t size = 0;
    size += p.print("[");
    size += p.print((int)type);
    size += p.print(" (");
    size += p.print(num1);
    size += p.print(", ");
    size += p.print(num2);
    size += p.print(")] ");
    size += p.print(data);
    return size;
}


Command parse(String string) {
    String intString1 = "";
    String intString2 = "";

    Serial.println("> " + string);
    Serial.print("Entered: ");

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

    // +2 from the substring index because we want after the space.
    String data = (type == CommandType::SetTime) ? string.substring(sizeof(commandString) + 2) : "";
    double number1 = intString1.toDouble();
    double number2 = intString2.toDouble();

    return Command(type, data, number1, number2);
}
}