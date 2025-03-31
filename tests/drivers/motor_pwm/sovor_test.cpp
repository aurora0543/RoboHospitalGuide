// main.cpp
#include "sovor.h"
#include "rpi_pwm.h" // 确保包含 RPI_PWM 的定义
#include <iostream>
#include <unistd.h>

int main() {
    int channel = 2;
    int frequency = 50;
    RPI_PWM pwm; // 实例化 RPI_PWM 对象

    // 初始化舵机
    servo_init(pwm, channel, frequency);

    // 示例：向左转 30 度
    servo_turn(pwm, 'L', 40);
    sleep(1);

    // 示例：向右转 20 度
    servo_turn(pwm, 'R', 40);
    sleep(1);

    // 舵机归中位
    servo_center(pwm);
    sleep(1);

    // 停止 PWM
    pwm.stop();

    return 0;
}
