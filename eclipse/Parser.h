#ifndef Parser_h
#define Parser_h

class Parser {
    public:
    class Command {
        /**
         * @brief 
         * e.g. goto 1 3
         * e.g. step 0 20 (move left 20 steps)
         * e.g. step 1 -30 (move right -30 steps)
         */
        public:

        enum class CommandType {
            MoveStep,
            MoveInch,
            Invalid
        };

        CommandType type;
        int num1;
        int num2;

        Command(CommandType t, int n1, int n2) {
            type = t;
            num1 = n1;
            num2 = n2;
        }
    };

    private:
    Command::CommandType processType(String commandString) {
        // if commandString.equals("step") {

        // }
    }

    int processInt(String intString) {

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
        Serial.println("Entered: " + string);

        int split1 = string.indexOf(" ");
        int split2 = string.indexOf(" ", split1 + 1);

        if (split1 < 0) {
            Serial.println("No space present");
        }

        String commandString = string.substring(0, split1);

        String intString1 = string.substring(split1 + 1, split2);
        String intString2 = string.substring(split2 + 1);

        Serial.println(commandString + "|" + intString1 + "|" + intString2);

        // Command::CommandType type = processType(commandString);
        // int number = processNumber(intString);

        // Serial.print("Command: ");
        // Serial.print((int)type);
        // Serial.print(" with number ");
        // Serial.println(number);

        return Command(Command::CommandType::Invalid, 0, 0);
    }
};

#endif