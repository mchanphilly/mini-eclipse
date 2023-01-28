#include <AccelStepper.h>

constexpr int leftStepPin = 3;
constexpr int leftDirPin = 6;
constexpr int enableMotorPin = 8;

static constexpr int stepsInMotor = 200;
static constexpr int microsteps = 8;
static constexpr double stepsPerRotation = stepsInMotor * microsteps;

static constexpr int secondsPerMinute = 60;

static constexpr double maxRotationsPerMinute = 500;
static constexpr double maxRotationsPerSecond = maxRotationsPerMinute / secondsPerMinute;
static constexpr double maxStepsPerSecond = maxRotationsPerSecond * stepsPerRotation;

static constexpr double rotationsPerMinute = 30;
static_assert(rotationsPerMinute <= maxRotationsPerMinute);
static constexpr double rotationsPerSecond = rotationsPerMinute / secondsPerMinute;

static constexpr double stepsPerSecond = rotationsPerSecond * stepsPerRotation;
// static constexpr double stepsPerSecond = 25600;

AccelStepper stepper = AccelStepper(AccelStepper::MotorInterfaceType::DRIVER, leftStepPin, leftDirPin);

void setup() {
    Serial.begin(9600);
    stepper.setPinsInverted(false, false, true);
    stepper.setEnablePin(enableMotorPin);
    
    constexpr bool isOn = true;
    if (isOn) {
        stepper.enableOutputs();

        stepper.setMaxSpeed(maxStepsPerSecond);
    } else {
        stepper.disableOutputs();
    }
    
    Serial.println("Example setup!");
    // Serial.p
    Serial.println(stepsPerRotation);
    Serial.println(maxStepsPerSecond);
    Serial.println(rotationsPerSecond);
    Serial.println(stepsPerSecond);
    stepper.move(stepsPerRotation);
    stepper.setSpeed(-stepsPerSecond);
    stepper.disableOutputs();
}

void loop() {
    // stepper.runSpeed();
    // stepper.move(stepsPerRotation);

    // stepper.runToPosition();
    stepper.runSpeedToPosition();
    // delay(5000);
    // Serial.println("Test");
    // Serial.println(stepsPerSecond);

    // delay(4000);
}