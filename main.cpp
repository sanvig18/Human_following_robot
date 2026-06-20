#include <iostream>
#include <chrono>
#include <thread>

struct RobotPins {
    static const int TRIG_PIN = 2;
    static const int ECHO_PIN = 3;
    static const int LEFT_IR_PIN = 4;
    static const int RIGHT_IR_PIN = 5;
    static const int MOTOR_LEFT_PWM = 9;
    static const int MOTOR_RIGHT_PWM = 10;
};

struct TrackingThresholds {
    static constexpr float MIN_FOLLOW_DIST_CM = 15.0f;
    static constexpr float MAX_FOLLOW_DIST_CM = 50.0f;
    static constexpr int BASE_SPEED = 150;
    static constexpr int MAX_SPEED = 255;
};

class HardwareInterface {
public:
    virtual void init() = 0;
    virtual float readUltrasonicDistance() = 0;
    virtual bool readLeftIR() = 0;
    virtual bool readRightIR() = 0;
    virtual void setMotorSpeeds(int leftSpeed, int rightSpeed) = 0;
    virtual ~HardwareInterface() = default;
};

class TargetTrackingEngine {
private:
    HardwareInterface& hw;
    int currentLeftSpeed = 0;
    int currentRightSpeed = 0;

    int computePwmResponse(float distance) {
        if (distance < TrackingThresholds::MIN_FOLLOW_DIST_CM) return 0;
        if (distance > TrackingThresholds::MAX_FOLLOW_DIST_CM) return 0;
        
        float factor = (distance - TrackingThresholds::MIN_FOLLOW_DIST_CM) / 
                       (TrackingThresholds::MAX_FOLLOW_DIST_CM - TrackingThresholds::MIN_FOLLOW_DIST_CM);
        
        return TrackingThresholds::BASE_SPEED + static_cast<int>(factor * (TrackingThresholds::MAX_SPEED - TrackingThresholds::BASE_SPEED));
    }

public:
    explicit TargetTrackingEngine(HardwareInterface& hardware) : hw(hardware) {}

    void executeUpdateCycle() {
        float distance = hw.readUltrasonicDistance();
        bool leftDetected = hw.readLeftIR();
        bool rightDetected = hw.readRightIR();

        if (distance < TrackingThresholds::MIN_FOLLOW_DIST_CM || distance > TrackingThresholds::MAX_FOLLOW_DIST_CM) {
            currentLeftSpeed = 0;
            currentRightSpeed = 0;
        } else {
            int targetSpeed = computePwmResponse(distance);

            if (leftDetected && !rightDetected) {
                currentLeftSpeed = targetSpeed / 2;
                currentRightSpeed = targetSpeed;
            } else if (!leftDetected && rightDetected) {
                currentLeftSpeed = targetSpeed;
                currentRightSpeed = targetSpeed / 2;
            } else {
                currentLeftSpeed = targetSpeed;
                currentRightSpeed = targetSpeed;
            }
        }

        hw.setMotorSpeeds(currentLeftSpeed, currentRightSpeed);
    }
};

class MockSimulationHardware : public HardwareInterface {
public:
    void init() override {}
    float readUltrasonicDistance() override { return 32.5f; }
    bool readLeftIR() override { return true; }
    bool readRightIR() override { return false; }
    void setMotorSpeeds(int leftSpeed, int rightSpeed) override {
        std::cout << "[Hardware Link] Motor Sync -> Left PWM: " << leftSpeed 
                  << " | Right PWM: " << rightSpeed << std::endl;
    }
};

int main() {
    MockSimulationHardware hardware;
    TargetTrackingEngine engine(hardware);

    hardware.init();
    std::cout << "[Kernel]: Human Following Logic Engine Engaged." << std::endl;

    for (int i = 0; i < 3; ++i) {
        engine.executeUpdateCycle();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}
