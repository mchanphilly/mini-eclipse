
#include "Executor.h"
#include "Parser.h"
// Mini-Eclipse project (January 2023)
// Martin Chan (philadelphia@mit.edu)

static constexpr int enableMotorPin {8};
const auto initialStrings = BlockerSystem::Tangential(double(8), double(42));

const auto initialCommand = Parser::Command(
  Parser::CommandType::SoftZero,
  initialStrings.left,
  initialStrings.right
);

Executor executor;
Parser parser;
String string;

void setup() {
  pinMode(enableMotorPin, OUTPUT);
  Serial.begin(9600);

  executor.init();
  executor.execute(initialCommand);
  Serial.println(executor.getState());
  }

void loop() {
  // Verify
  if (Serial.available()) {
    string = Serial.readStringUntil('\n');

    auto command = parser.parse(string);

    executor.execute(command);
    Serial.println(executor.getState());
  }

  executor.run();
}
