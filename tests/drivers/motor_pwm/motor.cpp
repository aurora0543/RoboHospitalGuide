#include "motor.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>

//////////////////////
// 构造与析构
//////////////////////
Motor::Motor(const MotorPins& pins, bool usePID, std::function<void(double)> callback)
    : chip(nullptr),
      line_AIN1(nullptr),
      line_AIN2(nullptr),
      line_BIN1(nullptr),
      line_BIN2(nullptr),
      encoder_left_A(nullptr),
      encoder_left_B(nullptr),
      encoder_right_A(nullptr),
      encoder_right_B(nullptr),
      running(true),
      left_duty(0),
      right_duty(0),
      left_direction(true),
      right_direction(true),
      target_speed_left(0.0),
      target_speed_right(0.0),
      left_count(0),
      right_count(0),
      speed_callback(callback),
      pins(pins),
      usePID(usePID)
{
    // 打开 GPIO 芯片
    chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) throw std::runtime_error("无法打开 GPIO 芯片");

    // 获取电机控制 GPIO 线
    line_AIN1 = gpiod_chip_get_line(chip, pins.AIN1);
    line_AIN2 = gpiod_chip_get_line(chip, pins.AIN2);
    line_BIN1 = gpiod_chip_get_line(chip, pins.BIN1);
    line_BIN2 = gpiod_chip_get_line(chip, pins.BIN2);
    if (!line_AIN1 || !line_AIN2 || !line_BIN1 || !line_BIN2)
        throw std::runtime_error("无法获取电机控制GPIO线");

    // 请求电机控制线为输出，并初始化为低电平
    gpiod_line_request_output(line_AIN1, "motor", 0);
    gpiod_line_request_output(line_AIN2, "motor", 0);
    gpiod_line_request_output(line_BIN1, "motor", 0);
    gpiod_line_request_output(line_BIN2, "motor", 0);

    // 如果使用 PID 模式，初始化编码器相关
    if (usePID) {
        encoder_left_A = gpiod_chip_get_line(chip, E1A);
        encoder_left_B = gpiod_chip_get_line(chip, E1B);
        encoder_right_A = gpiod_chip_get_line(chip, E2A);
        encoder_right_B = gpiod_chip_get_line(chip, E2B);
        if (!encoder_left_A || !encoder_left_B || !encoder_right_A || !encoder_right_B)
            throw std::runtime_error("无法获取编码器GPIO线");

        gpiod_line_request_input(encoder_left_A, "motor");
        gpiod_line_request_input(encoder_left_B, "motor");
        gpiod_line_request_input(encoder_right_A, "motor");
        gpiod_line_request_input(encoder_right_B, "motor");

        // 启动编码器采样线程
        left_encoder_thread = std::thread(&Motor::leftEncoderLoop, this);
        right_encoder_thread = std::thread(&Motor::rightEncoderLoop, this);

        // 启动 PID 调速线程
        left_pid_thread = std::thread(&Motor::leftPIDLoop, this);
        right_pid_thread = std::thread(&Motor::rightPIDLoop, this);
    }

    // PWM 线程始终启动（两轮独立）
    left_pwm_thread = std::thread(&Motor::pwmLoop, this, line_AIN1, line_AIN2, std::ref(left_duty), std::ref(left_direction));
    right_pwm_thread = std::thread(&Motor::pwmLoop, this, line_BIN1, line_BIN2, std::ref(right_duty), std::ref(right_direction));

    std::cout << "Motor driver initialized. Mode: " << (usePID ? "PID" : "Raw") << std::endl;
}

Motor::~Motor() {
    cleanup();
}

void Motor::cleanup() {
    running.store(false);
    // 等待各线程退出
    if (left_pwm_thread.joinable()) left_pwm_thread.join();
    if (right_pwm_thread.joinable()) right_pwm_thread.join();
    if (usePID) {
        if (left_encoder_thread.joinable()) left_encoder_thread.join();
        if (right_encoder_thread.joinable()) right_encoder_thread.join();
        if (left_pid_thread.joinable()) left_pid_thread.join();
        if (right_pid_thread.joinable()) right_pid_thread.join();
    }
    // 将所有输出置低
    if (line_AIN1) gpiod_line_set_value(line_AIN1, 0);
    if (line_AIN2) gpiod_line_set_value(line_AIN2, 0);
    if (line_BIN1) gpiod_line_set_value(line_BIN1, 0);
    if (line_BIN2) gpiod_line_set_value(line_BIN2, 0);

    // 释放资源
    if (line_AIN1) gpiod_line_release(line_AIN1);
    if (line_AIN2) gpiod_line_release(line_AIN2);
    if (line_BIN1) gpiod_line_release(line_BIN1);
    if (line_BIN2) gpiod_line_release(line_BIN2);
    if (usePID) {
        if (encoder_left_A) gpiod_line_release(encoder_left_A);
        if (encoder_left_B) gpiod_line_release(encoder_left_B);
        if (encoder_right_A) gpiod_line_release(encoder_right_A);
        if (encoder_right_B) gpiod_line_release(encoder_right_B);
    }
    if (chip) gpiod_chip_close(chip);

    std::cout << "Motor driver cleaned up." << std::endl;
}

//////////////////////
// PWM 及 PID 相关
//////////////////////

// pwmLoop：持续输出 PWM 波形，周期由 PWM_FREQUENCY 决定
void Motor::pwmLoop(gpiod_line* pin1, gpiod_line* pin2, std::atomic<int>& duty, std::atomic<bool>& direction) {
    int period_us = 1000000 / PWM_FREQUENCY;
    while (running.load()) {
        int currentDuty = duty.load();
        int high_time_us = (period_us * currentDuty) / PWM_RESOLUTION;
        bool dir = direction.load();
        // 输出方向信号
        gpiod_line_set_value(pin1, dir ? 1 : 0);
        gpiod_line_set_value(pin2, dir ? 0 : 1);
        if (high_time_us > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(high_time_us));
        if (currentDuty < 100) {
            gpiod_line_set_value(pin1, 0);
            gpiod_line_set_value(pin2, 0);
            std::this_thread::sleep_for(std::chrono::microseconds(period_us - high_time_us));
        }
    }
    // 停止时确保输出低电平
    gpiod_line_set_value(pin1, 0);
    gpiod_line_set_value(pin2, 0);
}

// 编码器轮询（左侧）：简单采样 encoder_left_A 的上升沿
void Motor::leftEncoderLoop() {
    int prev_state = gpiod_line_get_value(encoder_left_A);
    while (running.load()) {
        int state = gpiod_line_get_value(encoder_left_A);
        if (prev_state == 0 && state == 1) {
            left_count.fetch_add(1);
        }
        prev_state = state;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

// 编码器轮询（右侧）：简单采样 encoder_right_A 的上升沿
void Motor::rightEncoderLoop() {
    int prev_state = gpiod_line_get_value(encoder_right_A);
    while (running.load()) {
        int state = gpiod_line_get_value(encoder_right_A);
        if (prev_state == 0 && state == 1) {
            right_count.fetch_add(1);
        }
        prev_state = state;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

// 左侧 PID 控制，每 100ms 调整一次 PWM 占空比
void Motor::leftPIDLoop() {
    double integral = 0.0;
    double prev_error = 0.0;
    const double dt = 0.1; // 100ms
    while (running.load()) {
        int count = left_count.exchange(0);
        double measured_speed = count / dt;  // ticks/s
        double error = target_speed_left.load() - measured_speed;
        integral += error * dt;
        double derivative = (error - prev_error) / dt;
        double output = Kp * error + Ki * integral + Kd * derivative;
        int currentDuty = left_duty.load();
        int newDuty = currentDuty + static_cast<int>(output);
        if (newDuty < 0) newDuty = 0;
        if (newDuty > 100) newDuty = 100;
        left_duty.store(newDuty);
        prev_error = error;
        if (speed_callback)
            speed_callback(measured_speed);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 右侧 PID 控制
void Motor::rightPIDLoop() {
    double integral = 0.0;
    double prev_error = 0.0;
    const double dt = 0.1;
    while (running.load()) {
        int count = right_count.exchange(0);
        double measured_speed = count / dt;
        double error = target_speed_right.load() - measured_speed;
        integral += error * dt;
        double derivative = (error - prev_error) / dt;
        double output = Kp * error + Ki * integral + Kd * derivative;
        int currentDuty = right_duty.load();
        int newDuty = currentDuty + static_cast<int>(output);
        if (newDuty < 0) newDuty = 0;
        if (newDuty > 100) newDuty = 100;
        right_duty.store(newDuty);
        prev_error = error;
        if (speed_callback)
            speed_callback(measured_speed);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

//////////////////////
// 公共接口函数
//////////////////////

// PID 模式下：向前行驶，目标速度单位 ticks/s
void Motor::forward(double speed) {
    if (!usePID) {
        std::cerr << "当前为 Raw 模式，请调用 forwardRaw() 函数" << std::endl;
        return;
    }
    target_speed_left.store(speed);
    target_speed_right.store(speed);
    left_direction.store(true);   // 左轮正转
    right_direction.store(false); // 右轮反转（因安装相反）
}

// PID 模式下：向后行驶
void Motor::backward(double speed) {
    if (!usePID) {
        std::cerr << "当前为 Raw 模式，请调用 backwardRaw() 函数" << std::endl;
        return;
    }
    target_speed_left.store(speed);
    target_speed_right.store(speed);
    left_direction.store(false);   // 左轮反转
    right_direction.store(true);   // 右轮正转
}

// PID 模式下：停止
void Motor::stop() {
    if (!usePID) {
        std::cerr << "当前为 Raw 模式，请调用 stopRaw() 函数" << std::endl;
        return;
    }
    target_speed_left.store(0);
    target_speed_right.store(0);
    left_duty.store(0);
    right_duty.store(0);
    if (line_AIN1) gpiod_line_set_value(line_AIN1, 0);
    if (line_AIN2) gpiod_line_set_value(line_AIN2, 0);
    if (line_BIN1) gpiod_line_set_value(line_BIN1, 0);
    if (line_BIN2) gpiod_line_set_value(line_BIN2, 0);
}

// 原始 PWM 模式下：向前行驶，直接设置 PWM 占空比
void Motor::forwardRaw(int dutyCycle) {
    if (usePID) {
        std::cerr << "当前为 PID 模式，请调用 forward() 函数" << std::endl;
        return;
    }
    if (dutyCycle < 0 || dutyCycle > 100)
        throw std::runtime_error("占空比必须在 0-100之间");
    // 左轮正转，右轮反转
    left_direction.store(true);
    right_direction.store(false);
    left_duty.store(dutyCycle);
    right_duty.store(dutyCycle);
}

// 原始 PWM 模式下：向后行驶
void Motor::backwardRaw(int dutyCycle) {
    if (usePID) {
        std::cerr << "当前为 PID 模式，请调用 backward() 函数" << std::endl;
        return;
    }
    if (dutyCycle < 0 || dutyCycle > 100)
        throw std::runtime_error("占空比必须在 0-100之间");
    // 左轮反转，右轮正转
    left_direction.store(false);
    right_direction.store(true);
    left_duty.store(dutyCycle);
    right_duty.store(dutyCycle);
}

// 原始 PWM 模式下：停止
void Motor::stopRaw() {
    if (usePID) {
        std::cerr << "当前为 PID 模式，请调用 stop() 函数" << std::endl;
        return;
    }
    left_duty.store(0);
    right_duty.store(0);
    if (line_AIN1) gpiod_line_set_value(line_AIN1, 0);
    if (line_AIN2) gpiod_line_set_value(line_AIN2, 0);
    if (line_BIN1) gpiod_line_set_value(line_BIN1, 0);
    if (line_BIN2) gpiod_line_set_value(line_BIN2, 0);
}
