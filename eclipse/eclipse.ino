
#include "Executor.h"
#include "Parser.h"
#include "Scheduler.h"
// Mini-Eclipse project (January 2023)
// Martin Chan (philadelphia@mit.edu)

static constexpr bool verbose = true;

static constexpr int enableMotorPin {8};
const auto initialStrings = BlockerSystem::Tangential(double(8), double(42));

const auto initialCommand = Parser::Command(
  Parser::CommandType::SoftZero,
  initialStrings.left,
  initialStrings.right
);

String string;

void perform(Parser::Command command) {
  Executor::execute(command);
  if (verbose) Serial.println(command);
  Serial.println(Executor::getState());
}

void setup() {
  pinMode(enableMotorPin, OUTPUT);
  Serial.begin(9600);

  Executor::init();
  Executor::execute(initialCommand);
  Serial.println(Executor::getState());

  Scheduler::init("2023.02.06 18:33 -5", 10);
  Scheduler::setInterval(10);
  }

void loop() {
  // Verify
  if (Serial.available()) {
    string = Serial.readStringUntil('\n');

    auto command = Parser::parse(string);
    perform(command);
  }

  if (Scheduler::ready) {
    auto command = Scheduler::fetch();
    perform(command);
  }

  Executor::run();
  Scheduler::run();
}
