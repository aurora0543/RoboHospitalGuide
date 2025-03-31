#ifndef MOTOR_H
#define MOTOR_H

#include <functional>
#include <atomic>
#include <thread>
#include <gpiod.h>

// 编码器 GPIO 引脚定义
#define E1A 20
#define E1B 21
#define E2A 26
#define E2B 27


// 电机控制引脚配置结构体
struct MotorPins {
    unsigned int AIN1;  // 左轮控制引脚1
    unsigned int AIN2;  // 左轮控制引脚2
    unsigned int BIN1;  // 右轮控制引脚1
    unsigned int BIN2;  // 右轮控制引脚2
};

class Motor {
public:
    /**
     * @brief 构造函数
     * @param pins 电机控制引脚配置
     * @param usePID true: 使用 PID 调速（使用编码器反馈），false: 直接使用原始 PWM 模式
     * @param callback 可选速度回调函数（单位 ticks/s），仅在 PID 模式下有效
     */
    Motor(const MotorPins& pins, bool usePID = true, std::function<void(double)> callback = nullptr);
    ~Motor();

    // PID 模式下的接口，speed 单位为 ticks/s
    void forward(double speed);
    void backward(double speed);
    void stop();

    // 原始 PWM 模式下的接口，dutyCycle 直接表示 PWM 占空比（0~100）
    void forwardRaw(int dutyCycle);
    void backwardRaw(int dutyCycle);
    void stopRaw();

    // 清理资源
    void cleanup();

private:
    // PWM 循环：持续输出 PWM 信号
    void pwmLoop(gpiod_line* pin1, gpiod_line* pin2, std::atomic<int>& duty, std::atomic<bool>& direction);

    // PID 控制循环（仅在 PID 模式下启动）：每 100ms 计算一次误差并调整 PWM 占空比
    void leftPIDLoop();
    void rightPIDLoop();

    // 编码器轮询（仅在 PID 模式下启动）：简单采样编码器信号，统计 tick 数
    void leftEncoderLoop();
    void rightEncoderLoop();

private:
    // PWM 参数
    static constexpr int PWM_FREQUENCY = 1000;   // PWM 频率 (Hz)
    static constexpr int PWM_RESOLUTION = 100;     // 占空比分辨率

    // PID 参数（根据实际情况调节）
    static constexpr double Kp = 0.5;
    static constexpr double Ki = 0.1;
    static constexpr double Kd = 0.05;

    // GPIO 资源：电机输出
    gpiod_chip* chip;
    gpiod_line* line_AIN1;
    gpiod_line* line_AIN2;
    gpiod_line* line_BIN1;
    gpiod_line* line_BIN2;

    // 编码器 GPIO 资源（仅在 PID 模式下使用，作为输入）
    gpiod_line* encoder_left_A;
    gpiod_line* encoder_left_B;
    gpiod_line* encoder_right_A;
    gpiod_line* encoder_right_B;

    // 线程：PWM 输出始终运行；以下线程仅在 PID 模式下启动
    std::thread left_pwm_thread;
    std::thread right_pwm_thread;
    std::thread left_pid_thread;
    std::thread right_pid_thread;
    std::thread left_encoder_thread;
    std::thread right_encoder_thread;

    // 运行标志
    std::atomic<bool> running;

    // PWM 占空比和转向：分别针对左轮和右轮
    std::atomic<int> left_duty;
    std::atomic<int> right_duty;
    std::atomic<bool> left_direction;   // true 表示正转，false 表示反转
    std::atomic<bool> right_direction;  // true 表示正转，false 表示反转

    // PID 模式下的目标速度（单位 ticks/s）
    std::atomic<double> target_speed_left;
    std::atomic<double> target_speed_right;

    // 编码器计数（单位 ticks）
    std::atomic<int> left_count;
    std::atomic<int> right_count;

    // 速度回调函数（仅在 PID 模式下有效）
    std::function<void(double)> speed_callback;

    // 保存引脚配置
    MotorPins pins;

    // 模式选择：true 表示使用 PID 调速；false 表示原始 PWM 模式
    bool usePID;
};

#endif // MOTOR_H
