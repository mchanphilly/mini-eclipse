#include "Executor.h"

#include "Parser.h"
#include "Scheduler.h"
#include "MotorSystem.h"

namespace Executor {
using namespace Lengths;

namespace {

constexpr double goSteps {20};
int goIteration = goSteps;
TruePosition current{0, 0};
TruePosition step{0, 0};

void go(TruePosition truePosition) {
    const Radial radial(truePosition);
    const Tangential tangential(radial);
    const TotalLengths lengths(truePosition, radial, tangential);
    MotorSystem::setBearing(truePosition);
    MotorSystem::go(lengths);
}

void go(Position position) {
    const auto truePosition = TruePosition(position, MotorSystem::originOffset);
    go(truePosition);
}

void interpolateGo(Position position) {
    current = MotorSystem::getTruePosition();
    const auto destination = TruePosition(position, MotorSystem::originOffset);
    const auto delta = destination - current;
    step = TruePosition(delta.x / goSteps, delta.y / goSteps);
    goIteration = 0;
}

void checkGo() {
    if (!MotorSystem::stillMoving() && goIteration < goSteps) {
        current = current + step;
        goIteration++;
        go(current);
    }
}

TotalLengths syncMotors(Tangential tangential) {
    const Radial radial(tangential);
    const TruePosition truePosition(radial);
    const TotalLengths lengths(truePosition, radial, tangential);
}

void softZero(Lengths::Tangential tangential) {
    const auto lengths = syncMotors(tangential);
    MotorSystem::zero(lengths);
}

}

void execute(Parser::Command command) {
    // Serial.print("Command entered: ");
    Serial.println(command);
    double num1 = command.num1;
    double num2 = command.num2;

    double pair[2] {num1, num2};

    switch (command.type) {

        case Parser::CommandType::ShiftStep:
        MotorSystem::step(Steps(pair));
        break;

        case Parser::CommandType::ShiftInch:
        MotorSystem::step(TotalLengths(pair));
        break;

        case Parser::CommandType::GoStep:
        MotorSystem::go(Steps(pair));
        break;

        case Parser::CommandType::GoInch:
        MotorSystem::go(TotalLengths(pair));  // arbitrary length
        break;

        case Parser::CommandType::GetStep:
        // Note that this is very raw.
        Serial.println(MotorSystem::getSteps());
        break;

        case Parser::CommandType::GetInch:
        // Note that this DOES include the arc
        Serial.println(MotorSystem::getTangential());
        break;

        case Parser::CommandType::SoftZero:
        softZero(Tangential(pair));
        break;

        case Parser::CommandType::HardZero:
        // hardZero(Tangential(pair));
        Serial.println("Hardzero unavailable");
        break;

        case Parser::CommandType::Go:
        go(Lengths::Position(pair));
        break;

        case Parser::CommandType::GetPosition:
        Serial.println(MotorSystem::getPosition());
        break;

        case Parser::CommandType::SetTime:
        Scheduler::setTime(command.data);
        break;

        default:
        Serial.println("Bad command");
        break;
    }
}

void init(Tangential tangential) {
    // const String startTime = "2023.02.06 19:52 -5";
    const String startTime = "1970.01.01 05:00";
    constexpr double startZone = -5;
    constexpr double rate = 1;
    constexpr double interval = 1;

    Scheduler::init(startTime, rate);
    Scheduler::setZone(startZone);
    Scheduler::setInterval(interval);

    MotorSystem::init(tangential);
}

void run() {
    Scheduler::run();
    MotorSystem::run();
    checkGo();
    
    if (Scheduler::ready) {
        auto command = Scheduler::fetch();
        Serial.print("Scheduled: ");
        execute(command);
    }
}

}
