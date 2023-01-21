#ifndef Parser_h
#define Parser_h

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
    class Command {
        /**
         * @brief 
         * e.g. goto 1 3
         * e.g. step 0 20 (move left 20 steps)
         * e.g. step 1 -30 (move right -30 steps)
         */
        public:

        CommandType type;
        double num1;
        double num2;

        Command(CommandType t, double n1, double n2) {
            type = t;
            num1 = n1;
            num2 = n2;
        }
    };

    private:
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

        // Shouldn't get here if we had a valid command.
        return CommandType::Invalid;
    }

    public:
    

    /**
     * @brief 
     * 
     * @param string 
     * @return Command Two pieces of information:
     *      - An enum Parser::Command telling us what's needed
     *      - Either 0 or the relevant number. 
     */
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

        // Serial.println(commandString + "|" + intString1 + "|" + intString2);

        CommandType type = processType(commandString);
        double number1 = intString1.toDouble();
        double number2 = intString2.toDouble();

        // #ifdef DEBUG
        Serial.print((int)type);
        Serial.print("|");
        Serial.print(number1);
        Serial.print("|");
        Serial.println(number2);
        // #endif

        return Command(type, number1, number2);
    }
};

#endif