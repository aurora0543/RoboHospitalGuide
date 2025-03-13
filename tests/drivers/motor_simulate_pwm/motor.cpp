#include <gpiod.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>

// 全局指针，用于在信号处理程序中访问 PWM 对象
class PWM* pwmInstance = nullptr;

class PWM {
public:
    PWM(const std::string& chipName, unsigned int lineNum, int frequency, float dutyCycle)
        : chipName(chipName), lineNum(lineNum), frequency(frequency), dutyCycle(dutyCycle) {
        // 打开 GPIO 芯片
        chip = gpiod_chip_open_by_name(chipName.c_str());
        if (!chip) {
            throw std::runtime_error("无法打开 GPIO 芯片！");
        }

        // 获取 GPIO 线
        line = gpiod_chip_get_line(chip, lineNum);
        if (!line) {
            gpiod_chip_close(chip);
            throw std::runtime_error("无法获取 GPIO 线！");
        }

        // 申请输出模式
        if (gpiod_line_request_output(line, "PWM", 0) < 0) {
            gpiod_chip_close(chip);
            throw std::runtime_error("GPIO 输出请求失败！");
        }
    }

    ~PWM() {
        stop(); // 确保停止 PWM 并释放资源
    }

    void start() {
        running = true;
        int period_us = 1000000 / frequency;
        int high_time_us = static_cast<int>(period_us * dutyCycle);
        int low_time_us = period_us - high_time_us;

        while (running) {
            // 设置高电平
            gpiod_line_set_value(line, 1);
            std::this_thread::sleep_for(std::chrono::microseconds(high_time_us));

            // 设置低电平
            gpiod_line_set_value(line, 0);
            std::this_thread::sleep_for(std::chrono::microseconds(low_time_us));
        }

        // 确保在停止时引脚为低电平
        gpiod_line_set_value(line, 0);
    }

    void stop() {
        running = false;
    }

private:
    std::string chipName;
    unsigned int lineNum;
    int frequency;
    float dutyCycle;
    struct gpiod_chip* chip = nullptr;
    struct gpiod_line* line = nullptr;
    bool running = false;
};

// 信号处理函数
void handleSignal(int signal) {
    std::cout << "\n捕获到信号 " << signal << "，正在退出..." << std::endl;
    if (pwmInstance) {
        pwmInstance->stop();
    }
    exit(0);
}

int main() {
    try {
        // 设置信号处理函数
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);

        // 使用 GPIO 芯片 "gpiochip0" 的第 18 号引脚，频率为 1000Hz，占空比为 50%
        PWM pwm("gpiochip0", 18, 1000, 0.5);
        pwmInstance = &pwm; // 将 PWM 实例赋值给全局指针
        pwm.start();
        usleep(100000);

    return 0;
}
