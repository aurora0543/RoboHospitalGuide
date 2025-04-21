#include "nav.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <fstream>
#include <json.hpp>

namespace Nav {

std::atomic<bool> startNavigation(false);
std::atomic<bool> pauseNavigation(false);
std::mutex navMutex;
std::condition_variable navCV;

void checkPause(Motor& motor, UltrasonicSensor& sensor, float safeDistance) {
    std::unique_lock<std::mutex> lock(navMutex);
    while (pauseNavigation.load()) {
        motor.stop();
        std::cout << "Motor stopped, navigation is paused...\n";
        navCV.wait(lock);
        std::cout << "Navigation resumed, continuing...\n";
        motor.forward(30);
    }

    // 获取当前距离
    float distance = sensor.getDistance();  // 假设你有一个方法可以获取传感器的距离值
    // 如果距离小于安全距离，则停止
    if (distance < safeDistance) {
        motor.stop();
        std::cout << "Obstacle detected, stopping motor...\n";
        while (distance < safeDistance) {
            distance = sensor.getDistance();  // 更新距离
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "Obstacle cleared, continuing...\n";
        motor.forward(30);  // 继续前进
    }
}

void moveForward(Motor& motor, int duration_ms, UltrasonicSensor& sensor, float safeDistance) {
    std::cout << "Moving forward for " << duration_ms << " ms...\n";
    motor.forward(40);
    int elapsed = 0;

    while (elapsed < duration_ms) {
        checkPause(motor, sensor, safeDistance);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        elapsed += 100;
    }

    motor.stop();
    std::cout << "Finished moving forward\n";
}

void turnLeft(Motor& motor, Servo& servo, YawTracker& yaw, float angle, UltrasonicSensor& sensor, float safeDistance) {
    servo.turn('L', 45);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    yaw.start(50);
    float startAngle = yaw.getAngle();

    std::cout << "Turning left " << angle << " degrees...\n";
    motor.forward(40);
    while (true) {
        checkPause(motor, sensor, safeDistance);

        float currentAngle = yaw.getAngle();
        if (std::abs(currentAngle - startAngle) >= angle) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    motor.stop();
    std::cout << "Left turn completed\n";
    servo.center();
}

void turnRight(Motor& motor, Servo& servo, YawTracker& yaw, float angle, UltrasonicSensor& sensor, float safeDistance) {
    servo.turn('R', 45);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    yaw.start(40);
    float startAngle = yaw.getAngle();

    std::cout << "Turning right " << angle << " degrees...\n";
    motor.forward(50);
    while (true) {
        checkPause(motor, sensor, safeDistance);

        float currentAngle = yaw.getAngle();
        if (std::abs(currentAngle - startAngle) >= angle) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    motor.stop();
    std::cout << "Right turn completed\n";
    servo.center();
}

void navigationThread(Motor* motor, Servo* servo, YawTracker* yaw, const std::string& target, nlohmann::json navJson, UltrasonicSensor& sensor, float safeDistance) {
    Motor& m = *motor;
    Servo& s = *servo;
    YawTracker& y = *yaw;

    std::cout << "\nStarting navigation → Target department: " << target << "\n";

    if (!navJson.contains(target)) {
        std::cerr << "Navigation path not found for department \"" << target << "\"\n";
        return;
    }

    if (!navJson[target].contains("path") || !navJson[target]["path"].is_array()) {
        std::cerr << "Invalid or missing navigation data (\"path\") for \"" << target << "\"\n";
        return;
    }

    for (const auto& step : navJson[target]["path"]) {
        if (!step.contains("action") || !step.contains("value")) {
            std::cerr << "Malformed navigation step: missing 'action' or 'value'\n";
            continue;
        }
        std::string action = step["action"];
        int value = step["value"];

        if (action == "moveForward") {
            moveForward(m, value, sensor, safeDistance);
        } else if (action == "turnLeft") {
            turnLeft(m, s, y, value, sensor, safeDistance);
        } else if (action == "turnRight") {
            turnRight(m, s, y, value, sensor, safeDistance);
        } else {
            std::cerr << "Unknown navigation action: " << action << "\n";
        }
    }

    std::cout << "Navigation completed!\n";
}

}  // namespace Nav
