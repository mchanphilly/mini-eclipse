#include "Executor.h"

#include "Blocker.h"
#include "Parser.h"
#include "Scheduler.h"
#include "MotorSystem.h"

namespace Executor {
using namespace Blocker;

namespace {
StringState state;

inline void printPosition() {
    Serial.println(state.toPosition());
}

inline void printTangential() {
    Serial.println(state.toTangential());
}

void go(Lengths::Position position) {
    state.go(position);
    const auto lengths = state.toTotalLengths();
    MotorSystem::go(lengths.left, lengths.right, MotorSystem::Unit::Inch);
}

void syncMotors(Lengths::Tangential tangential) {
    const auto lengths = state.toTotalLengths();
    MotorSystem::zero(lengths.left, lengths.right);
}

void softZero(Lengths::Tangential tangential) {
    state.softZero(tangential);
    syncMotors(tangential);
}

void hardZero(Lengths::Tangential tangential) {
    state.hardZero(tangential);
    syncMotors(tangential);
}

int counter {0};
const int pace {10000};
void runMotors() {
    // counter = (counter + 1) % pace;
    StringSpeed newSpeed = state.getSpeed();

    // if (counter == 0) {
        MotorSystem::run(newSpeed);
    // } else {
    //     MotorSystem::run();
    // }
}
}

void execute(Parser::Command command) {
    // Serial.print("Command entered: ");
    Serial.println(command);
    double num1 = command.num1;
    double num2 = command.num2;

    double pair[2] {num1, num2};
    double lengths[2];

    // If only this was C++17 and not C++11
    // using enum Parser::CommandType;

    MotorSystem::Unit unit = MotorSystem::Unit::Invalid;
    switch (command.type) {
        case Parser::CommandType::ShiftStep:
        case Parser::CommandType::GoStep:
        case Parser::CommandType::GetStep:
        unit = MotorSystem::Unit::Step;
        break;

        case Parser::CommandType::ShiftInch:
        case Parser::CommandType::GoInch:
        case Parser::CommandType::GetInch:
        unit = MotorSystem::Unit::Inch;
        break;
    }

    switch (command.type) {

        case Parser::CommandType::ShiftStep:
        case Parser::CommandType::ShiftInch:
        MotorSystem::step(num1, num2, unit);
        break;

        case Parser::CommandType::GoStep:
        case Parser::CommandType::GoInch:
        MotorSystem::go(num1, num2, unit);
        break;

        case Parser::CommandType::GetStep:
        // Note that this is very raw.
        Serial.println(MotorSystem::getSteps());
        break;

        case Parser::CommandType::GetInch:
        // Note that this does NOT include the arc
        printTangential();
        break;

        case Parser::CommandType::SoftZero:
        softZero(Lengths::Tangential(pair));
        break;

        case Parser::CommandType::HardZero:
        hardZero(Lengths::Tangential(pair));
        break;

        case Parser::CommandType::Go:
        go(Lengths::Position(pair));
        break;

        case Parser::CommandType::GetPosition:
        printPosition();
        break;

        case Parser::CommandType::SetTime:
        Scheduler::setTime(command.data);
        break;

        default:
        Serial.println("Bad command");
        break;
    }
}

void init() {
    // const String startTime = "2023.02.06 19:52 -5";
    const String startTime = "1970.01.01 05:00";
    constexpr double startZone = -5;
    constexpr double rate = 1;
    constexpr double interval = 1;

    Scheduler::init(startTime, rate);
    Scheduler::setZone(startZone);
    Scheduler::setInterval(interval);

    MotorSystem::init();
}

void run() {
    Scheduler::run();

    runMotors();
    
    if (Scheduler::ready) {
        auto command = Scheduler::fetch();
        Serial.print("Scheduled: ");
        execute(command);
    }
}

const StringState& getState() {
    return state;
}

}
