#include "servo.h"
#include "pwm.h"  // 和 Motor 一样的库
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cmath>

#define PWM_PERIOD_US 20000
#define CENTER_DUTY_US 1500
#define LEFT_MIN_DUTY_US 600
#define RIGHT_MAX_DUTY_US 2400
#define MAX_ANGLE 90

Servo::Servo(int gpio_pin)
    : chip(nullptr),
      line(nullptr),
      running(true),
      duty_us(CENTER_DUTY_US),
      pin(gpio_pin)
{
    chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) throw std::runtime_error("无法打开 GPIO 芯片");

    line = gpiod_chip_get_line(chip, pin);
    if (!line) throw std::runtime_error("无法获取 GPIO 线");

    if (gpiod_line_request_output(line, "servo", 0) < 0)
        throw std::runtime_error("无法设置 GPIO 为输出");

    pwm_thread = std::thread(&Servo::pwmLoop, this, line, std::ref(duty_us));
    std::cout << "Servo initialized on GPIO pin " << pin << std::endl;
}

Servo::~Servo() {
    running.store(false);
    if (pwm_thread.joinable()) pwm_thread.join();

    gpiod_line_set_value(line, 0);
    gpiod_line_release(line);
    if (chip) gpiod_chip_close(chip);

    std::cout << "Servo cleaned up." << std::endl;
}

void Servo::pwmLoop(struct gpiod_line* line, std::atomic<int>& duty_cycle) {
    while (running.load()) {
        int high_time = duty_cycle.load();  // us
        int low_time = PWM_PERIOD_US - high_time;

        gpiod_line_set_value(line, 1);
        usleep(high_time);

        gpiod_line_set_value(line, 0);
        usleep(low_time);
    }
}

void Servo::center() {
    duty_us.store(CENTER_DUTY_US);
    std::cout << "舵机归中，占空比: " << CENTER_DUTY_US << "us" << std::endl;
    usleep(500000);
}

void Servo::turn(char direction, int angle) {
    if (angle < 0) angle = 0;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;

    float ratio = static_cast<float>(angle) / MAX_ANGLE;
    int target_duty;

    if (direction == 'L' || direction == 'l') {
        target_duty = static_cast<int>(CENTER_DUTY_US - ratio * (CENTER_DUTY_US - LEFT_MIN_DUTY_US) + 0.5f);
    } else if (direction == 'R' || direction == 'r') {
        target_duty = static_cast<int>(CENTER_DUTY_US + ratio * (RIGHT_MAX_DUTY_US - CENTER_DUTY_US) + 0.5f);
    } else {
        std::cout << "无效方向，请用 'L' 或 'R'" << std::endl;
        return;
    }

    duty_us.store(target_duty);
    std::cout << "舵机向 " << (direction == 'L' || direction == 'l' ? "左" : "右")
              << " 转动 " << angle << "°，占空比设置为 " << target_duty << "us" << std::endl;

    usleep(500000);
}