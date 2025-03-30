#include <gpiod.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <csignal>
#include <functional> // 包含 std::function

// 电机控制引脚（根据你的接线定义）
// 为了方便管理，建议使用一个结构体来保存引脚
struct MotorPins {
    unsigned int AIN1;
    unsigned int AIN2;
    unsigned int BIN1;
    unsigned int BIN2;
};

// PWM 参数
constexpr int PWM_FREQUENCY = 1000;    // PWM 频率 (Hz)
constexpr int PWM_RESOLUTION = 100;    // 占空比分辨率 (1-100)

// 全局变量
static struct gpiod_chip* chip = nullptr;
static struct gpiod_line* line_AIN1 = nullptr;
static struct gpiod_line* line_AIN2 = nullptr;
static struct gpiod_line* line_BIN1 = nullptr;
static struct gpiod_line* line_BIN2 = nullptr;
static std::atomic<bool> running{true};
static std::thread motorA_thread;
static std::thread motorB_thread;
static std::function<void(int)> speed_changed_callback; // 回调函数

// 信号处理函数 (内部使用)
static void signalHandler(int signal) {
    std::cout << "\n停止电机并清理 GPIO..." << std::endl;
    running = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待线程退出

    // 设置所有引脚为低电平
    if (line_AIN1) gpiod_line_set_value(line_AIN1, 0);
    if (line_AIN2) gpiod_line_set_value(line_AIN2, 0);
    if (line_BIN1) gpiod_line_set_value(line_BIN1, 0);
    if (line_BIN2) gpiod_line_set_value(line_BIN2, 0);

    // 释放资源
    if (line_AIN1) gpiod_line_release(line_AIN1);
    if (line_AIN2) gpiod_line_release(line_AIN2);
    if (line_BIN1) gpiod_line_release(line_BIN1);
    if (line_BIN2) gpiod_line_release(line_BIN2);
    if (chip) gpiod_chip_close(chip);

    exit(0);
}

// PWM 控制线程函数 (内部使用)
static void pwmControl(int dutyCycle, bool forward, gpiod_line* pin1, gpiod_line* pin2) {
    int period_us = 1000000 / PWM_FREQUENCY;    // 周期（微秒）
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

// 初始化函数
void MotorDriverInit(const MotorPins& pins, std::function<void(int)> callback) {
    // 注册信号处理函数
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 保存回调函数
    speed_changed_callback = callback;

    try {
        chip = gpiod_chip_open_by_name("gpiochip0");
        if (!chip) throw std::runtime_error("无法打开 GPIO 芯片！");

        line_AIN1 = gpiod_chip_get_line(chip, pins.AIN1);
        line_AIN2 = gpiod_chip_get_line(chip, pins.AIN2);
        line_BIN1 = gpiod_chip_get_line(chip, pins.BIN1);
        line_BIN2 = gpiod_chip_get_line(chip, pins.BIN2);

        if (!line_AIN1 || !line_AIN2 || !line_BIN1 || !line_BIN2) {
            throw std::runtime_error("无法获取 GPIO 线！");
        }

        // 申请输出模式
        gpiod_line_request_output(line_AIN1, "motor", 0);
        gpiod_line_request_output(line_AIN2, "motor", 0);
        gpiod_line_request_output(line_BIN1, "motor", 0);
        gpiod_line_request_output(line_BIN2, "motor", 0);

        std::cout << "电机驱动初始化完成！" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "初始化错误: " << e.what() << std::endl;
        // 可以在这里添加一些错误处理代码，例如抛出异常或返回错误码
        throw; // 重新抛出异常，让调用者处理
    }
}

// 启动电机控制
void StartMotorControl(int dutyCycleA, bool motorA_forward, int dutyCycleB, bool motorB_forward) {
     if (dutyCycleA < 0 || dutyCycleA > 100 || dutyCycleB < 0 || dutyCycleB > 100) {
        throw std::runtime_error("占空比必须在 0-100 之间！");
    }
    // 停止之前的电机线程
    if (motorA_thread.joinable()) {
        running = false;
        motorA_thread.join();
        running = true; // 重新设置为 true，以便后续使用
    }
    if (motorB_thread.joinable()) {
        running = false;
        motorB_thread.join();
        running = true;
    }

    // 启动 PWM 线程
    motorA_thread = std::thread(pwmControl, dutyCycleA, motorA_forward, line_AIN1, line_AIN2);
    motorB_thread = std::thread(pwmControl, dutyCycleB, motorB_forward, line_BIN1, line_BIN2);
    if (speed_changed_callback)
    {
        speed_changed_callback(dutyCycleA + dutyCycleB);
    }
    std::cout << "电机已启动，速度 A: " << dutyCycleA << "%, 方向 A: " << (motorA_forward ? "前进" : "后退")
              << ", 速度 B: " << dutyCycleB << "%, 方向 B: " << (motorB_forward ? "前进" : "后退") << std::endl;
}

// 停止电机控制
void StopMotorControl() {
    running = false; // 停止 PWM 线程
    if (motorA_thread.joinable()) {
        motorA_thread.join();
    }
    if (motorB_thread.joinable()) {
        motorB_thread.join();
    }

    // 设置所有引脚为低电平，确保电机停止
    if (line_AIN1) gpiod_line_set_value(line_AIN1, 0);
    if (line_AIN2) gpiod_line_set_value(line_AIN2, 0);
    if (line_BIN1) gpiod_line_set_value(line_BIN1, 0);
    if (line_BIN2) gpiod_line_set_value(line_BIN2, 0);
    std::cout << "电机已停止！" << std::endl;
}

// 释放资源的函数
void MotorDriverCleanup() {
    running = false; // 确保线程停止
     if (motorA_thread.joinable()) {
        motorA_thread.join();
    }
    if (motorB_thread.joinable()) {
        motorB_thread.join();
    }
    // 释放资源
    if (line_AIN1) gpiod_line_release(line_AIN1);
    if (line_AIN2) gpiod_line_release(line_AIN2);
    if (line_BIN1) gpiod_line_release(line_BIN1);
    if (line_BIN2) gpiod_line_release(line_BIN2);
    if (chip) gpiod_chip_close(chip);

    std::cout << "GPIO 资源已清理！" << std::endl;
}

int main() {
    // 定义电机引脚
    MotorPins motor_pins = {
        17, // 电机 A 方向 1
        18, // 电机 A 方向 2
        22, // 电机 B 方向 1
        23  // 电机 B 方向 2
    };

    // 定义一个回调函数
    auto onSpeedChanged = [](int speed) {
        std::cout << "电机速度已改变，总速度: " << speed << std::endl;
        // 在这里可以添加你的处理逻辑，例如更新 UI
    };

    // 初始化电机驱动
    try {
        MotorDriverInit(motor_pins, onSpeedChanged); // 传入引脚和回调函数
    } catch (const std::exception& e) {
        std::cerr << "初始化失败，程序退出！" << e.what() << std::endl;
        return 1;
    }
    // 启动电机，设置速度和方向
    StartMotorControl(50, true, 30, false); // 电机 A: 50% 正转，电机 B: 30% 反转

    // 模拟主程序的其他操作
    std::cout << "主程序正在运行..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 改变电机速度和方向
    std::cout << "改变电机速度和方向..." << std::endl;
    StartMotorControl(80, false, 60, true); // 电机 A: 80% 反转，电机 B: 60% 正转
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 停止电机
    StopMotorControl();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 释放资源
    MotorDriverCleanup();

    return 0;
}
