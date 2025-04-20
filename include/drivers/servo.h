// #ifndef SERVO_H
// #define SERVO_H

// #include <gpiod.h>
// #include <thread>
// #include <atomic>

// class Servo {
// public:
//     Servo(int gpio_pin);
//     ~Servo();

//     void center();
//     void turn(char direction, int angle);

// private:
//     void pwmLoop(struct gpiod_line* line, std::atomic<int>& duty_cycle);

//     struct gpiod_chip* chip;
//     struct gpiod_line* line;
//     std::thread pwm_thread;
//     std::atomic<bool> running;
//     std::atomic<int> duty_us;
//     int pin;
// };

// #endif // SERVO_H

#ifndef SERVO_H
#define SERVO_H

#include <gpiod.h>
#include <atomic>
#include <thread>

class Servo {
public:
    explicit Servo(int gpio_pin);
    ~Servo();

    void center();
    void turn(char direction, int angle);

private:
    void pwmLoop(struct gpiod_line* line, std::atomic<int>& duty_cycle);

    struct gpiod_chip* chip;
    struct gpiod_line* line;
    std::atomic<bool> running;
    std::atomic<int> duty_us;
    std::thread pwm_thread;
    int pin;
};

#endif  // SERVO_H