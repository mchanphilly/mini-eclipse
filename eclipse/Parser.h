#ifndef Parser_h
#define Parser_h

#include <Arduino.h>

namespace Parser {

enum class CommandType {
        Invalid,
        Test,  // test [testNumber] (executes a test in Test) [UNUSED]
        
        Stop, // stop whatever motion
        Start,  // begin checking scheduled tasks
        Pause,  // pause scheduled tasks

        SetTime,  // read the string data

        GetStep,  // getstep (gets steps from the zero)
        GoStep,  // gostep [str1] [str2]
        ShiftStep,  // step [str1] [str2] (moves the string by these steps)

        GetInch,  // getinch (get string inches from the zero)
        GoInch,  // goinch [str1] [str2] ()
        ShiftInch,  // inch [str1] [str2] (moves the string by these inches)

        GetPosition,  // getpos (get x, y coordinates)
        Go, // go [x] [y]
        
        HardZero, // origin [str1] [str2] (make this new origin) Warning: not robust
        SoftZero, // fix [str1] [str2] (adjust str lengths and position to match; keeps old origin)
};

class Command : public Printable {
public:
    Command(CommandType t, String data, double n1, double n2);

    size_t printTo(Print& p) const;

    CommandType type;
    double num1;
    double num2;
    String data;
};

/**
 * @brief 
 * 
 * @param string 
 * @return Command Two pieces of information:
 *      - An enum Parser::Command telling us what's needed
 *      - Either 0 or the relevant number. 
 */
Command parse(String string);

const Command empty {CommandType::Invalid, "", 0, 0};
}

#endif