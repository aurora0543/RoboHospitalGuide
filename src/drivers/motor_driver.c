/**
 * @file motor_driver.c
 * @brief Motor driver for hospital robot project
 * 
 * 
 * @author [Your Name]
 * @date 2025-02-16
 * @license MIT
 */

// motor_driver.c
#include "motor_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

// PWM 配置
#define PWM_CHIP "/sys/class/pwm/pwmchip0"
#define PWM0 PWM_CHIP "/pwm0"
#define PWM1 PWM_CHIP "/pwm1"
#define PERIOD "1000000"  // 1ms = 1000000ns
#define MAX_DUTY 1000000  // 占空比最大值

// 全局状态
static uint8_t current_speed = 0;
static MotorDirection current_dir = MOTOR_STOP;

// 隐藏内部函数
static int file_exists(const char* path);
static void precise_delay_ms(unsigned int ms);
static int write_sysfs(const char* path, const char* value);
static void pwm_init(void);
static void set_pwm(int ch0_duty, int ch1_duty);

// 初始化PWM
static void pwm_init() {
    // 清理可能存在的旧设备
    if (file_exists(PWM0)) system("echo 0 > " PWM_CHIP "/unexport");
    if (file_exists(PWM1)) system("echo 1 > " PWM_CHIP "/unexport");

    // 导出设备
    write_sysfs(PWM_CHIP "/export", "0");
    write_sysfs(PWM_CHIP "/export", "1");

    // 等待设备就绪
    int retries = 0;
    while ((!file_exists(PWM0 "/duty_cycle") || 
           !file_exists(PWM1 "/duty_cycle")) && 
           retries++ < 100) {
        precise_delay_ms(10);
    }

    // 配置PWM
    write_sysfs(PWM0 "/period", PERIOD);
    write_sysfs(PWM1 "/period", PERIOD);
    write_sysfs(PWM0 "/enable", "1");
    write_sysfs(PWM1 "/enable", "1");
}

// 根据方向设置PWM
static void update_motor() {
    int duty = current_speed * MAX_DUTY / 100;
    
    switch(current_dir) {
    case MOTOR_FORWARD:
        set_pwm(duty, duty);
        break;
    case MOTOR_BACKWARD:
        set_pwm(-duty, -duty);
        break;
    case MOTOR_STOP:
        set_pwm(0, 0);1
        break;
    }
}

// 公有接口实现
void motor_init(void) {
    pwm_init();
    motor_set_direction(MOTOR_STOP);
}

void motor_set_speed(uint8_t speed) {
    if (speed > 100) speed = 100;
    current_speed = speed;
    update_motor();
}

void motor_set_direction(MotorDirection direction) {
    current_dir = direction;
    update_motor();
}

// 以下为底层PWM操作函数
static int file_exists(const char* path) {
    struct stat st;
    return (stat(path, &st) == 0);
}

static void precise_delay_ms(unsigned int ms) {
    struct timespec ts = {
        .tv_sec = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000L
    };
    nanosleep(&ts, NULL);
}

static int write_sysfs(const char* path, const char* value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("无法打开PWM设备");
        return -1;
    }
    if (write(fd, value, strlen(value)) < 0) {
        perror("写入PWM参数失败");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

static void set_pwm(int ch0_duty, int ch1_duty) {
    char duty_str[16];
    
    // 设置通道0
    snprintf(duty_str, sizeof(duty_str), "%d", ch0_duty);
    if (write_sysfs(PWM0 "/duty_cycle", duty_str) != 0) {
        fprintf(stderr, "PWM0设置失败\n");
    }

    // 设置通道1
    snprintf(duty_str, sizeof(duty_str), "%d", ch1_duty);
    if (write_sysfs(PWM1 "/duty_cycle", duty_str) != 0) {
        fprintf(stderr, "PWM1设置失败\n");
    }
}