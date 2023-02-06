#ifndef Parser_h
#define Parser_h

#include <Arduino.h>

class Parser {
public:
    enum class CommandType {
            Invalid,
            Test,  // test [testNumber] (executes a test in Test) [UNUSED]

            GetStep,  // getstep (gets steps from the zero)
            GoStep,  // gostep [str1] [str2]
            ShiftStep,  // step [str1] [str2] (moves the string by these steps)

            GetInch,  // getinch (get string inches from the zero)
            GoInch,  // goinch [str1] [str2] ()
            ShiftInch,  // inch [str1] [str2] (moves the string by these inches)

            GetPosition,  // getpos (get x, y coordinates)
            Go, // go [x] [y]
            Shift, // shift [x] [y]
            
            HardZero, // origin [str1] [str2] (declare current position as (0, 0) with str lengths in inches)
            SoftZero, // fix [str1] [str2] (adjust str lengths and position to match; keeps old origin)
    };

    class Command : public Printable {
    public:
        Command(CommandType t, double n1, double n2);

        size_t printTo(Print& p) const;

        CommandType type;
        double num1;
        double num2;
    };

    /**
     * @brief 
     * 
     * @param string 
     * @return Command Two pieces of information:
     *      - An enum Parser::Command telling us what's needed
     *      - Either 0 or the relevant number. 
     */
    static Command parse(String string);

    static const Command empty;

private:
    static CommandType processType(String commandString);
};

// const Parser::Command empty = Parser::Command(Parser::CommandType::Invalid, 0, 0);

#endif