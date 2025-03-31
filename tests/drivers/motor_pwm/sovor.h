// servo_controller.h
#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include "rpi_pwm.h" // 假设 rpi_pwm.h 已经存在，并且包含了 RPI_PWM 类的定义

// 参数定义：这些值需要根据实际舵机和控制电路校准
#define CENTER_DUTY    7   // 舵机中位对应的占空比
#define LEFT_MIN_DUTY  5   // 向左极限对应的占空比
#define RIGHT_MAX_DUTY 10  // 向右极限对应的占空比
#define MAX_ANGLE      45  // 最大转角（单位：度）

/**
 * @brief 初始化舵机控制。启动 PWM 并将舵机设置到中心位置。
 * @param pwm RPI_PWM 类的实例，用于控制 PWM。
 * @param channel PWM 通道号。
 * @param frequency PWM 频率，单位 Hz。
 */
void servo_init(RPI_PWM &pwm, int channel, int frequency);

/**
 * @brief 将舵机设置到中心位置。
 * @param pwm RPI_PWM 类的实例。
 */
void servo_center(RPI_PWM &pwm);

/**
 * @brief 控制舵机转动到指定角度。
 * @param pwm RPI_PWM 类的实例。
 * @param direction 转动方向，'L' 或 'l' 表示左转，'R' 或 'r' 表示右转。
 * @param angle 转动角度，单位度。
 */
void servo_turn(RPI_PWM &pwm, char direction, int angle);

#endif // SERVO_CONTROLLER_H