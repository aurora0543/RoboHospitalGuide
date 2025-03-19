#include <gpiod.hpp>
#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <atomic>
#include <sys/time.h>

#define TRIG_PIN  24  // GPIO 23
#define ECHO_PIN  25  // GPIO 24

// 获取当前时间（微秒）
long getMicrotime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

class UltrasonicSensor {
public:
    using Callback = std::function<void(float)>;
    UltrasonicSensor(const std::string& chip_name, int trig_pin, int echo_pin, Callback callback)
        : running(true), callback(callback) {
        chip = std::make_unique<gpiod::chip>(chip_name);
        trig_line = chip->get_line(trig_pin);
        echo_line = chip->get_line(echo_pin);
        trig_line.request({"ultrasonic", gpiod::line_request::DIRECTION_OUTPUT, 0}, 0);
        echo_line.request({"ultrasonic", gpiod::line_request::DIRECTION_INPUT, 0});
        sensor_thread = std::thread(&UltrasonicSensor::measureDistance, this);
    }

    ~UltrasonicSensor() {
        running = false;
        if (sensor_thread.joinable()) {
            sensor_thread.join();
        }
    }

private:
    std::unique_ptr<gpiod::chip> chip;
    gpiod::line trig_line;
    gpiod::line echo_line;
    std::atomic<bool> running;
    std::thread sensor_thread;
    Callback callback;

    void measureDistance() {
        while (running) {
            // 发送超声波脉冲
            trig_line.set_value(0);
            std::this_thread::sleep_for(std::chrono::microseconds(2));
            trig_line.set_value(1);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            trig_line.set_value(0);

            // 等待回声信号
            while (echo_line.get_value() == 0);
            long startTime = getMicrotime();
            while (echo_line.get_value() == 1);
            long travelTime = getMicrotime() - startTime;

            // 计算距离（cm）
            float distance = travelTime * 0.0343f / 2.0f;
            callback(distance);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

int main() {
    std::cout << "HC-SR04 Ultrasonic Sensor Test Started..." << std::endl;

    UltrasonicSensor sensor("gpiochip0", TRIG_PIN, ECHO_PIN, [](float distance) {
        std::cout << "Measured Distance: " << distance << " cm" << std::endl;
        if (distance < 20.0) {
            std::cout << "⚠ Warning: Obstacle too close! Stop or turn!" << std::endl;
        }
    });

    std::this_thread::sleep_for(std::chrono::minutes(1)); // 运行 1 分钟

    return 0;
}