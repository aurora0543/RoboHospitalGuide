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

void checkPause(Motor& motor) {
    std::unique_lock<std::mutex> lock(navMutex);
    while (pauseNavigation.load()) {
        motor.stop();
        std::cout << "⏸️ 电机停止，导航已挂起...\n";
        navCV.wait(lock);
        std::cout << "▶️ 导航已恢复，继续执行...\n";
        motor.forward(30);
    }
}

void moveForward(Motor& motor, int duration_ms) {
    std::cout << "⬆️  前进 " << duration_ms << " 毫秒...\n";
    motor.forward(40);
    int elapsed = 0;
    while (elapsed < duration_ms) {
        checkPause(motor);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        elapsed += 100;
    }
    motor.stop();
    std::cout << "🛑 前进结束\n";
}

void turnLeft(Motor& motor, Servo& servo, YawTracker& yaw, float angle) {
    servo.turn('L', 45);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    yaw.start(50);
    float startAngle = yaw.getAngle();
    
    std::cout << "↪️ 左转 " << angle << " 度...\n";
    motor.forward(40);
    while (true) {
        checkPause(motor);
        float currentAngle = yaw.getAngle();
        if (std::abs(currentAngle - startAngle) >= angle) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    motor.stop();
    std::cout << "✅ 左转完成\n";
    servo.center();
}



void turnRight(Motor& motor, Servo& servo, YawTracker& yaw, float angle) {
    servo.turn('R', 45);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    yaw.start(40);
    float startAngle = yaw.getAngle();
    
    std::cout << "↩️ 右转 " << angle << " 度...\n";
    motor.forward(50);
    while (true) {
        checkPause(motor);
        float currentAngle = yaw.getAngle();
        if (std::abs(currentAngle - startAngle) >= angle) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    motor.stop();
    std::cout << "✅ 右转完成\n";
    servo.center();
}



void navigationThread(Motor* motor, Servo* servo, YawTracker* yaw, const std::string& target, nlohmann::json navJson) {
    Motor& m = *motor;
    Servo& s = *servo;
    YawTracker& y = *yaw;

    std::cout << "\n🚦 开始导航 → 目标科室: " << target << "\n";

    if (!navJson.contains(target)) {
        std::cerr << "❌ 未找到目标科室 \"" << target << "\" 的导航路径\n";
        return;
    }

    if (!navJson[target].contains("path") || !navJson[target]["path"].is_array()) {
        std::cerr << "❌ \"" << target << "\" 的导航数据无效或缺少 path\n";
        return;
    }

    for (const auto& step : navJson[target]["path"]) {
        if (!step.contains("action") || !step.contains("value")) {
            std::cerr << "❌ 导航步骤格式错误，缺少 action 或 value\n";
            continue;
        }
        std::string action = step["action"];
        int value = step["value"];

        if (action == "moveForward") {
            moveForward(m, value);
        } else if (action == "turnLeft") {
            turnLeft(m, s, y, value);
        } else if (action == "turnRight") {
            turnRight(m, s, y, value);
        } else {
            std::cerr << "❌ 未知导航动作: " << action << "\n";
        }
    }

    std::cout << "🏁 导航完成！\n";
}

}  // namespace Nav