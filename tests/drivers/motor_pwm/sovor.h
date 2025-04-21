// servo_controller.h
#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include "rpi_pwm.h" // 假设 rpi_pwm.h 已经存在，并且包含了 RPI_PWM 类的定义

#define CENTER_DUTY    7   // 舵机中位对应的占空比
#define LEFT_MIN_DUTY  5   // 向左极限对应的占空比
#define RIGHT_MAX_DUTY 10  // 向右极限对应的占空比
#define MAX_ANGLE      45  // 最大转角（单位：度）

void servo_init(RPI_PWM &pwm, int channel, int frequency);


void servo_center(RPI_PWM &pwm);


void servo_turn(RPI_PWM &pwm, char direction, int angle);

#endif // SERVO_CONTROLLER_H