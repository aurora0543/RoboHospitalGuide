#ifndef MOTOR_H
#define MOTOR_H

#include <atomic>
#include <gpiod.h>
#include "pwm.h"
#include <thread>

struct MotorPins {
    int AIN1;
    int AIN2;
    int BIN1;
    int BIN2;
};

class Motor {
public:
    Motor(const MotorPins& pins);
    ~Motor();

    void forward(int dutyCycle);
    void backward(int dutyCycle);
    void stop();

    void pwmLoop(gpiod_line* pin1, gpiod_line* pin2, std::atomic<int>& duty, std::atomic<bool>& direction);

private:
    gpiod_chip* chip;
    gpiod_line* line_AIN1;
    gpiod_line* line_AIN2;
    gpiod_line* line_BIN1;
    gpiod_line* line_BIN2;

    std::atomic<bool> running;
    std::atomic<int> left_duty;
    std::atomic<int> right_duty;
    std::atomic<bool> left_direction;
    std::atomic<bool> right_direction;

    MotorPins pins;
    std::thread left_pwm_thread;
    std::thread right_pwm_thread;

    void cleanup();
};

#endif