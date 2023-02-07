#include "Executor.h"

#include "BlockerSystem.h"
#include "Parser.h"
#include "Scheduler.h"
#include "MotorSystem.h"

namespace Executor {
namespace {
BlockerSystem blocker;
MotorSystem motors;

inline void printPosition() {
    Serial.println(blocker.getStringState().toPosition());
}

inline void printTangential() {
    Serial.println(blocker.getStringState().toTangential());
}

void go(BlockerSystem::Position position) {
    blocker.update(position);
    const auto state = blocker.getStringState();
    const auto lengths = state.toTotalLengths();

    motors.go(lengths.left, lengths.right, MotorSystem::Unit::Inch);
}

void zero(BlockerSystem::Tangential tangential) {
    blocker.softZero(tangential);
    const auto state = blocker.getStringState();
    const auto lengths = state.toTotalLengths();

    motors.zero(lengths.left, lengths.right);
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
        motors.step(num1, num2, unit);
        break;

        case Parser::CommandType::GoStep:
        case Parser::CommandType::GoInch:
        motors.go(num1, num2, unit);
        break;

        case Parser::CommandType::GetStep:
        // Note that this is very raw.
        motors.getLengths(lengths, unit);
        //   printPair(lengths);
        Serial.println("Print lengths for GetStep not available");
        break;

        case Parser::CommandType::GetInch:
        // Note that this does NOT include the arc
        printTangential();
        break;

        case Parser::CommandType::SoftZero:
        zero(BlockerSystem::Tangential(pair));
        break;

        case Parser::CommandType::Go:
        go(BlockerSystem::Position(pair));
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

    motors.init();
}

void run() {
    Scheduler::run();
    motors.run();
    if (Scheduler::ready) {
        auto command = Scheduler::fetch();
        Serial.print("Scheduled: ");
        execute(command);
    }
}

const BlockerSystem::StringState& getState() {
    return blocker.getStringState();
}

}
