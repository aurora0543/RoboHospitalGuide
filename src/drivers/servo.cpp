#include "servo.h"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cmath>
#include <pthread.h>

#define PWM_PERIOD_US 20000
#define CENTER_DUTY_US 1500
#define LEFT_MIN_DUTY_US 600
#define RIGHT_MAX_DUTY_US 2400
#define MAX_ANGLE 90

Servo::Servo(int gpio_pin)
    : chip(nullptr),
      line(nullptr),
      running(true),
      duty_us(CENTER_DUTY_US),
      pin(gpio_pin)
{
    chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) throw std::runtime_error("Failed to open GPIO chip");

    line = gpiod_chip_get_line(chip, pin);
    if (!line) throw std::runtime_error("Failed to get GPIO line");

    if (gpiod_line_request_output(line, "servo", 0) < 0)
        throw std::runtime_error("Failed to set GPIO as output");

    pwm_thread = std::thread(&Servo::pwmLoop, this, line, std::ref(duty_us));
    std::cout << "Servo initialized on GPIO pin " << pin << std::endl;
}

Servo::~Servo() {
    running.store(false);
    if (pwm_thread.joinable()) pwm_thread.join();

    gpiod_line_set_value(line, 0);
    gpiod_line_release(line);
    if (chip) gpiod_chip_close(chip);

    std::cout << "Servo cleanup complete." << std::endl;
}

void Servo::pwmLoop(struct gpiod_line* line, std::atomic<int>& duty_cycle) {
    // Set real-time thread scheduling (may require root privileges)
    struct sched_param sch_params;
    sch_params.sched_priority = 80;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params) != 0) {
        std::cerr << "Warning: Failed to set real-time thread (may require root permissions)" << std::endl;
    }

    // Optional: Bind this thread to CPU core 2 to reduce migration
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);  // Adjust core number if necessary
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    int cached_duty = duty_cycle.load();
    int update_counter = 0;

    while (running.load()) {
        int high_time = cached_duty;
        int low_time = PWM_PERIOD_US - high_time;

        gpiod_line_set_value(line, 1);
        usleep(high_time);

        gpiod_line_set_value(line, 0);
        usleep(low_time);

        // Update duty cycle every 10 periods to reduce jitter
        if (++update_counter % 10 == 0) {
            cached_duty = duty_cycle.load();
        }
    }
}

void Servo::center() {
    duty_us.store(CENTER_DUTY_US);
    std::cout << "Servo centered. Duty cycle: " << CENTER_DUTY_US << " us" << std::endl;
    usleep(500000);  // Delay for stabilization
}

void Servo::turn(char direction, int angle) {
    if (angle < 0) angle = 0;
    if (angle > MAX_ANGLE) angle = MAX_ANGLE;

    float ratio = static_cast<float>(angle) / MAX_ANGLE;
    int target_duty;

    if (direction == 'L' || direction == 'l') {
        target_duty = static_cast<int>(CENTER_DUTY_US - ratio * (CENTER_DUTY_US - LEFT_MIN_DUTY_US) + 0.5f);
    } else if (direction == 'R' || direction == 'r') {
        target_duty = static_cast<int>(CENTER_DUTY_US + ratio * (RIGHT_MAX_DUTY_US - CENTER_DUTY_US) + 0.5f);
    } else {
        std::cout << "Invalid direction. Please use 'L' or 'R'." << std::endl;
        return;
    }

    duty_us.store(target_duty);
    std::cout << "Servo turning " << (direction == 'L' ? "left" : "right")
              << " by " << angle << " degrees. Duty cycle set to " << target_duty << " us" << std::endl;

    usleep(500000);  // Delay to allow movement
}