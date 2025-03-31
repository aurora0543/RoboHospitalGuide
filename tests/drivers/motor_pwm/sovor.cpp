// servo_controller.cpp
#include "sovor.h"
#include <stdio.h>
#include <unistd.h>

void servo_init(RPI_PWM &pwm, int channel, int frequency) {
    pwm.start(channel, frequency);
    pwm.setDutyCycle(CENTER_DUTY);
    printf("PWM 在通道 %d 启动，舵机初始化至中位 (占空比: %d)\n", channel, CENTER_DUTY);
    usleep(50000); // 延时 50 毫秒
}

void servo_center(RPI_PWM &pwm) {
    pwm.setDutyCycle(CENTER_DUTY);
    printf("舵机归中位 (占空比: %d)\n", CENTER_DUTY);
    usleep(500000); // 延时 500 毫秒
}

void servo_turn(RPI_PWM &pwm, char direction, int angle) {
    // 限制角度范围
    if (angle < 0) {
        angle = 0;
    }
    if (angle > MAX_ANGLE) {
        angle = MAX_ANGLE;
    }

    // 归一化比例 (0 ~ 1)
    float fraction = (float)angle / (float)MAX_ANGLE;
    int newDuty;

    if (direction == 'L' || direction == 'l') {
        // 从中位向左减小占空比
        newDuty = (int)(CENTER_DUTY - fraction * (CENTER_DUTY - LEFT_MIN_DUTY) + 0.5f);
        printf("向左转 %d°: 占空比从 %d 调整到 %d\n", angle, CENTER_DUTY, newDuty);
    } else if (direction == 'R' || direction == 'r') {
        // 从中位向右增加占空比
        newDuty = (int)(CENTER_DUTY + fraction * (RIGHT_MAX_DUTY - CENTER_DUTY) + 0.5f);
        printf("向右转 %d°: 占空比从 %d 调整到 %d\n", angle, CENTER_DUTY, newDuty);
    } else {
        printf("无效的方向参数，请输入 'L' 或 'R'\n");
        return;
    }

    pwm.setDutyCycle(newDuty);
    usleep(500000); // 延时 500 毫秒
}