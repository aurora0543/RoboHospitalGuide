#include "motor.h"

void Motor::pwmLoop(gpiod_line* pin1, gpiod_line* pin2, std::atomic<int>& duty, std::atomic<bool>& direction) {
    int period_us = 1000000 / PWM_FREQUENCY;
    while (running.load()) {
        int currentDuty = duty.load();
        int high_time_us = (period_us * currentDuty) / PWM_RESOLUTION;
        bool dir = direction.load();

        gpiod_line_set_value(pin1, dir ? 1 : 0);
        gpiod_line_set_value(pin2, dir ? 0 : 1);

        if (high_time_us > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(high_time_us));
        if (currentDuty < 100) {
            gpiod_line_set_value(pin1, 0);
            gpiod_line_set_value(pin2, 0);
            std::this_thread::sleep_for(std::chrono::microseconds(period_us - high_time_us));
        }
    }
    gpiod_line_set_value(pin1, 0);
    gpiod_line_set_value(pin2, 0);
}