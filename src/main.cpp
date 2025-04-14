#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

#include "motor.h"
#include "servo.h"
#include "yaw_tracker.h"
#include "mpu6050.h"

// 控制标志与同步机制
std::atomic<bool> startNavigation(false);
std::atomic<bool> pauseNavigation(false);
std::mutex navMutex;
std::condition_variable navCV;

// 暂停检查函数（暂停时停止电机）
void checkPause(Motor& motor)
{
    std::unique_lock<std::mutex> lock(navMutex);
    while (pauseNavigation.load()) {
        motor.stop();  // ⚠️ 暂停时立即停止电机
        std::cout << "⏸️ 电机停止，导航已挂起...\n";
        navCV.wait(lock);  // 等待 resume
        std::cout << "▶️ 导航已恢复，继续执行...\n";
        motor.forward(30);  // 可视情况恢复电机动作（可选）
    }
}

// 前进函数
void moveForward(Motor& motor, int duration_ms)
{
    std::cout << "⬆️  前进 " << duration_ms << " 毫秒...\n";
    motor.forward(30);
    int elapsed = 0;

    while (elapsed < duration_ms) {
        checkPause(motor);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        elapsed += 100;
    }

    motor.stop();
    std::cout << "🛑 前进结束\n";
}

// 左转函数
void turnLeft(Motor& motor, Servo& servo, YawTracker& yaw, float angle)
{
    servo.turn('L', 45);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    yaw.start(50);
    float startAngle = yaw.getAngle();

    std::cout << "↪️ 左转 " << angle << " 度...\n";
    motor.forward(30);

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

// 右转函数
void turnRight(Motor& motor, Servo& servo, YawTracker& yaw, float angle)
{
    servo.turn('R', 45);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    yaw.start(50);
    float startAngle = yaw.getAngle();

    std::cout << "↩️ 右转 " << angle << " 度...\n";
    motor.forward(30);

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

// 导航线程函数
void navigationThread(Motor* motorPtr, Servo* servoPtr, YawTracker* yawPtr)
{
    Motor& motor = *motorPtr;
    Servo& servo = *servoPtr;
    YawTracker& yaw = *yawPtr;

    while (true) {
        if (startNavigation) {
            std::cout << "\n🚦 开始导航...\n";

            moveForward(motor, 1000);
            turnLeft(motor, servo, yaw, 30);
            moveForward(motor, 1000);
            turnRight(motor, servo, yaw, 30);

            std::cout << "🏁 导航完成！\n";
            startNavigation = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 主程序入口
int main()
{
    MotorPins pins = { 17, 16, 22, 23 };

    try {
        Motor motor(pins);
        Servo servo(18);
        YawTracker yaw;

        // 启动导航线程
        std::thread navThread(navigationThread, &motor, &servo, &yaw);

        // 终端命令控制
        std::string command;
        std::cout << "📟 控制命令：start | pause | resume | exit\n";

        while (true) {
            std::cout << "> ";
            std::cin >> command;

            if (command == "start") {
                if (!startNavigation) {
                    startNavigation = true;
                    pauseNavigation = false;
                    navCV.notify_all();  // 解除暂停
                } else {
                    std::cout << "⚠️ 导航已在进行中\n";
                }
            } else if (command == "pause") {
                if (startNavigation && !pauseNavigation) {
                    pauseNavigation = true;
                    std::cout << "⏸️ 导航已挂起\n";
                } else {
                    std::cout << "⚠️ 当前未导航或已暂停\n";
                }
            } else if (command == "resume") {
                if (pauseNavigation) {
                    pauseNavigation = false;
                    navCV.notify_all();
                    std::cout << "▶️ 导航已恢复\n";
                } else {
                    std::cout << "⚠️ 当前未处于暂停状态\n";
                }
            } else if (command == "exit") {
                std::cout << "👋 程序退出，导航线程将自动终止\n";
                break;
            } else {
                std::cout << "❓ 未知命令: " << command << "\n";
            }
        }

        navThread.detach();  // 或 join()，视你的应用设计而定

    } catch (const std::exception& ex) {
        std::cerr << "❌ 错误: " << ex.what() << std::endl;
    }

    return 0;
}