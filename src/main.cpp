#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include "motor.h"
#include "servo.h"
#include "yaw_tracker.h"
#include "nav.h"
#include "json.hpp"  // nlohmann::json 的头文件

int main() {
    MotorPins pins = { 17, 16, 22, 23 };

    try {
        Motor motor(pins);
        Servo servo(18);
        YawTracker yaw;

        // 在 main 中读取导航 JSON 配置文件
        nlohmann::json navJson;
        std::ifstream navFile("../config/nav.json");
        if (navFile.is_open()) {
            navFile >> navJson;
            navFile.close();
        } else {
            std::cerr << "❌ 无法打开导航配置文件\n";
            return 1;
        }

        // 启动导航线程，将读取到的 navJson 传递给 navigationThread
        std::thread navThread(Nav::navigationThread, &motor, &servo, &yaw, navJson);

        // 终端命令控制
        std::string command;
        std::cout << "📟 控制命令：start | pause | resume | exit\n";

        while (true) {
            std::cout << "> ";
            std::cin >> command;
            
            if (command == "start") {
                if (!Nav::startNavigation.load()) {
                    Nav::startNavigation.store(true);
                    Nav::pauseNavigation.store(false);
                    Nav::navCV.notify_all();  // 解除暂停
                } else {
                    std::cout << "⚠️ 导航已在进行中\n";
                }
            } else if (command == "pause") {
                if (Nav::startNavigation.load() && !Nav::pauseNavigation.load()) {
                    Nav::pauseNavigation.store(true);
                    std::cout << "⏸️ 导航已挂起\n";
                } else {
                    std::cout << "⚠️ 当前未导航或已暂停\n";
                }
            } else if (command == "resume") {
                if (Nav::pauseNavigation.load()) {
                    Nav::pauseNavigation.store(false);
                    Nav::navCV.notify_all();
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

        navThread.detach();  // 或根据实际需求选择 join()

    } catch (const std::exception& ex) {
        std::cerr << "❌ 错误: " << ex.what() << std::endl;
    }
    
    return 0;
}