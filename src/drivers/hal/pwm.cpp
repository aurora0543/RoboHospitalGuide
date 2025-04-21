#include "motor.h"

void Motor::pwmLoop(gpiod_line* pin1, gpiod_line* pin2, std::atomic<int>& duty, std::atomic<bool>& direction) {
    const int period_us = 10000;  // 100 Hz

    while (running.load()) {
        int d = duty.load();
        bool dir = direction.load();
        int high_time = (period_us * d) / 100;
        int low_time = period_us - high_time;

        // 设置方向
        gpiod_line_set_value(pin1, dir ? 1 : 0);
        gpiod_line_set_value(pin2, dir ? 0 : 1);

        // 高电平部分
        if (high_time > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(high_time));

        // 低电平部分
        gpiod_line_set_value(pin1, 0);
        gpiod_line_set_value(pin2, 0);

        if (low_time > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(low_time));
    }

    // 停止时关闭引脚
    gpiod_line_set_value(pin1, 0);
    gpiod_line_set_value(pin2, 0);
}