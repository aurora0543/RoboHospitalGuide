#include "motor.h"
#include "pwm.h"
#include <stdexcept>
#include <iostream>

Motor::Motor(const MotorPins& pins)
    : chip(nullptr),
      line_AIN1(nullptr),
      line_AIN2(nullptr),
      line_BIN1(nullptr),
      line_BIN2(nullptr),
      running(true),
      left_duty(0),
      right_duty(0),
      left_direction(true),
      right_direction(true),
      pins(pins)
{
    chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) throw std::runtime_error("无法打开 GPIO 芯片");

    line_AIN1 = gpiod_chip_get_line(chip, pins.AIN1);
    line_AIN2 = gpiod_chip_get_line(chip, pins.AIN2);
    line_BIN1 = gpiod_chip_get_line(chip, pins.BIN1);
    line_BIN2 = gpiod_chip_get_line(chip, pins.BIN2);

    if (!line_AIN1 || !line_AIN2 || !line_BIN1 || !line_BIN2)
        throw std::runtime_error("无法获取电机控制GPIO线");

    gpiod_line_request_output(line_AIN1, "motor", 0);
    gpiod_line_request_output(line_AIN2, "motor", 0);
    gpiod_line_request_output(line_BIN1, "motor", 0);
    gpiod_line_request_output(line_BIN2, "motor", 0);

    left_pwm_thread = std::thread(&Motor::pwmLoop, this, line_AIN1, line_AIN2, std::ref(left_duty), std::ref(left_direction));
    right_pwm_thread = std::thread(&Motor::pwmLoop, this, line_BIN1, line_BIN2, std::ref(right_duty), std::ref(right_direction));

    std::cout << "Motor driver initialized." << std::endl;
}

Motor::~Motor() {
    cleanup();
}

void Motor::cleanup() {
    running.store(false);
    if (left_pwm_thread.joinable()) left_pwm_thread.join();
    if (right_pwm_thread.joinable()) right_pwm_thread.join();

    gpiod_line_set_value(line_AIN1, 0);
    gpiod_line_set_value(line_AIN2, 0);
    gpiod_line_set_value(line_BIN1, 0);
    gpiod_line_set_value(line_BIN2, 0);

    gpiod_line_release(line_AIN1);
    gpiod_line_release(line_AIN2);
    gpiod_line_release(line_BIN1);
    gpiod_line_release(line_BIN2);

    if (chip) gpiod_chip_close(chip);
    std::cout << "Motor driver cleaned up." << std::endl;
}

void Motor::forward(int dutyCycle) {
    if (dutyCycle < 0 || dutyCycle > 100)
        throw std::runtime_error("the duty cycle must be between 0-100");
    left_direction.store(true);
    right_direction.store(false);
    left_duty.store(dutyCycle);
    right_duty.store(dutyCycle);
}

void Motor::backward(int dutyCycle) {
    if (dutyCycle < 0 || dutyCycle > 100)
        throw std::runtime_error("the duty cycle must be between 0-100");
    left_direction.store(false);
    right_direction.store(true);
    left_duty.store(dutyCycle);
    right_duty.store(dutyCycle);
}

void Motor::stop() {
    left_duty.store(0);
    right_duty.store(0);
    gpiod_line_set_value(line_AIN1, 0);
    gpiod_line_set_value(line_AIN2, 0);
    gpiod_line_set_value(line_BIN1, 0);
    gpiod_line_set_value(line_BIN2, 0);
}