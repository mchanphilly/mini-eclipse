
#include "Executor.h"
#include "Parser.h"
#include "Lengths.h"
// Mini-Eclipse project (January 2023)
// Martin Chan (philadelphia@mit.edu)

static constexpr bool verbose = true;

static constexpr int enableMotorPin {8};
const auto initialStrings = Lengths::Tangential(double(8), double(42));

String string;

void setup() {
  pinMode(enableMotorPin, OUTPUT);
  Serial.begin(9600);

  Executor::init(initialStrings);
  // Serial.println(Executor::getState());
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
