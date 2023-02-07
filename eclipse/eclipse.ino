
#include "Executor.h"
#include "Parser.h"
// Mini-Eclipse project (January 2023)
// Martin Chan (philadelphia@mit.edu)

static constexpr bool verbose = true;

static constexpr int enableMotorPin {8};
const auto initialStrings = Blocker::Tangential(double(8), double(42));

const auto initialCommand = Parser::Command(
  Parser::CommandType::HardZero,
  "",
  initialStrings.left,
  initialStrings.right
);

String string;

void setup() {
  pinMode(enableMotorPin, OUTPUT);
  Serial.begin(9600);

  Executor::init();
  Executor::execute(initialCommand);
  Serial.println(Executor::getState());
  }

void loop() {
  // Just for unscheduled commands
  if (Serial.available()) {
    string = Serial.readStringUntil('\n');

    auto command = Parser::parse(string);
    Executor::execute(command);
  }
  Executor::run();
}
