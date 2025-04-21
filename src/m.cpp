#include "nav.h"
#include <QApplication>
#include "mainwindow.h"  // RobotGUI
#include <thread>
#include "record.h"
#include <gpiod.hpp>  // 需要包含 gpiod 来使用 UltrasonicSensor 类

#define TRIG_PIN  24  // GPIO 24
#define ECHO_PIN  25  // GPIO 25

// 获取当前时间（微秒）
long getMicrotime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // 创建 UltrasonicSensor 实例
    UltrasonicSensor sensor("gpiochip0", TRIG_PIN, ECHO_PIN, [](int status) {
        if (status == 0) {
            std::cout << "⚠ Obstacle detected, stopping the car!" << std::endl;
            // 这里可以调用暂停小车运行的代码，例如调用停止函数
        } else if (status == 1) {
            std::cout << "Obstacle cleared, continuing the car's movement!" << std::endl;
            // 这里可以调用继续小车运行的代码，例如调用前进函数
        }
    });

    // 加载导航数据
    std::ifstream navFile("navigation.json");
    nlohmann::json navJson;
    navFile >> navJson;

    // 设定安全距离（单位：厘米）
    float safeDistance = 20.0f;

    // 创建导航线程
    Motor motor;  // 需要根据实际情况定义 Motor 类
    Servo servo;  // 需要根据实际情况定义 Servo 类
    YawTracker yaw;  // 需要根据实际情况定义 YawTracker 类
    std::thread navThread(Nav::navigationThread, &motor, &servo, &yaw, "targetDepartment", navJson, std::ref(sensor), safeDistance);

    navThread.join();  // 等待导航线程完成

    return a.exec();
}
