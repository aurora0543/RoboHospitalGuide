#include <gpiod.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <csignal>

// 电机控制引脚（根据你的接线）
constexpr unsigned int AIN1 = 17;  // 电机A方向1
constexpr unsigned int AIN2 = 18;  // 电机A方向2
constexpr unsigned int BIN1 = 22;  // 电机B方向1
constexpr unsigned int BIN2 = 23;  // 电机B方向2

// PWM 参数
constexpr int PWM_FREQUENCY = 1000;     // PWM 频率 (Hz)
constexpr int PWM_RESOLUTION = 100;     // 占空比分辨率 (1-100)

// 全局变量
struct gpiod_chip* chip = nullptr;
struct gpiod_line* line_AIN1 = nullptr;
struct gpiod_line* line_AIN2 = nullptr;
struct gpiod_line* line_BIN1 = nullptr;
struct gpiod_line* line_BIN2 = nullptr;
std::atomic<bool> running{true};

// 信号处理函数
void signalHandler(int signal) {
    std::cout << "\n停止电机并清理 GPIO..." << std::endl;
    running = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待线程退出

    // 设置所有引脚为低电平
    gpiod_line_set_value(line_AIN1, 0);
    gpiod_line_set_value(line_AIN2, 0);
    gpiod_line_set_value(line_BIN1, 0);
    gpiod_line_set_value(line_BIN2, 0);

    // 释放资源
    if (line_AIN1) gpiod_line_release(line_AIN1);
    if (line_AIN2) gpiod_line_release(line_AIN2);
    if (line_BIN1) gpiod_line_release(line_BIN1);
    if (line_BIN2) gpiod_line_release(line_BIN2);
    if (chip) gpiod_chip_close(chip);

    exit(0);
}

// 初始化 GPIO
void setupGPIO() {
    chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) throw std::runtime_error("无法打开 GPIO 芯片！");

    line_AIN1 = gpiod_chip_get_line(chip, AIN1);
    line_AIN2 = gpiod_chip_get_line(chip, AIN2);
    line_BIN1 = gpiod_chip_get_line(chip, BIN1);
    line_BIN2 = gpiod_chip_get_line(chip, BIN2);

    if (!line_AIN1 || !line_AIN2 || !line_BIN1 || !line_BIN2) {
        throw std::runtime_error("无法获取 GPIO 线！");
    }

    // 申请输出模式
    gpiod_line_request_output(line_AIN1, "motor", 0);
    gpiod_line_request_output(line_AIN2, "motor", 0);
    gpiod_line_request_output(line_BIN1, "motor", 0);
    gpiod_line_request_output(line_BIN2, "motor", 0);
}

// PWM 控制线程函数
void pwmControl(int dutyCycle, bool forward, gpiod_line* pin1, gpiod_line* pin2) {
    int period_us = 1000000 / PWM_FREQUENCY;  // 周期（微秒）
    int high_time_us = (period_us * dutyCycle) / PWM_RESOLUTION;

    while (running) {
        // 设置方向
        gpiod_line_set_value(pin1, forward ? 1 : 0);
        gpiod_line_set_value(pin2, forward ? 0 : 1);

        // 高电平时间
        if (high_time_us > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(high_time_us));
        }

        // 低电平时间（占空比 < 100% 时生效）
        if (dutyCycle < 100) {
            gpiod_line_set_value(pin1, 0);
            gpiod_line_set_value(pin2, 0);
            std::this_thread::sleep_for(std::chrono::microseconds(period_us - high_time_us));
        }
    }

    // 停止电机
    gpiod_line_set_value(pin1, 0);
    gpiod_line_set_value(pin2, 0);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        setupGPIO();

        // 设置两个电机的速度和方向（示例：电机A正转50%速度，电机B反转30%速度）
        int dutyCycleA = 50;  // 占空比 0-100
        int dutyCycleB = 30;
        bool motorA_forward = true;
        bool motorB_forward = false;

        // 启动 PWM 线程
        std::thread motorA(pwmControl, dutyCycleA, motorA_forward, line_AIN1, line_AIN2);
        std::thread motorB(pwmControl, dutyCycleB, motorB_forward, line_BIN1, line_BIN2);

        std::cout << "电机已启动，按 Ctrl+C 停止..." << std::endl;

        motorA.join();
        motorB.join();
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}