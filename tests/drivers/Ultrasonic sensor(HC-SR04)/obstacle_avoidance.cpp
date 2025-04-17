#include "motor.h"
#include "servo.h"
#include "yaw_tracker.h"
#include <gpiod.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <iostream>
#include <condition_variable>

std::atomic<bool> running(true);
std::atomic<bool> paused(false);
std::mutex pauseMutex;
std::condition_variable pauseCV;
float frontDistance = 1000.0f;
std::mutex distanceMutex;

class UltrasonicSensor {
public:
    UltrasonicSensor(const std::string& chip_name, int trig_pin, int echo_pin)
        : running(true) {
        chip = std::make_unique<gpiod::chip>(chip_name);
        trig_line = chip->get_line(trig_pin);
        echo_line = chip->get_line(echo_pin);
        trig_line.request({"ultrasonic", gpiod::line_request::DIRECTION_OUTPUT, 0}, 0);
        echo_line.request({"ultrasonic", gpiod::line_request::DIRECTION_INPUT, 0});
        sensor_thread = std::thread(&UltrasonicSensor::measureDistance, this);
    }
    
    ~UltrasonicSensor() {
        running = false;
        if (sensor_thread.joinable()) sensor_thread.join();
    }
    
private:
    std::unique_ptr<gpiod::chip> chip;
    gpiod::line trig_line;
    gpiod::line echo_line;
    std::atomic<bool> running;
    std::thread sensor_thread;
    
    long getMicrotime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000000 + tv.tv_usec;
    }
    
    void measureDistance() {
        while (running) {
            // 触发超声波发射
            trig_line.set_value(0);
            std::this_thread::sleep_for(std::chrono::microseconds(2));
            trig_line.set_value(1);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            trig_line.set_value(0);
            
            // 等待回波信号，添加超时保护
            auto start_wait = std::chrono::steady_clock::now();
            while (echo_line.get_value() == 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_wait).count();
                if (elapsed > 30) break; // 超时保护，30ms
            }
            
            // 测量回波持续时间
            long start = getMicrotime();
            
            start_wait = std::chrono::steady_clock::now();
            while (echo_line.get_value() == 1) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_wait).count();
                if (elapsed > 30) break; // 超时保护，30ms
            }
            
            long travel = getMicrotime() - start;
            float distance = travel * 0.0343f / 2.0f; // 声速343m/s
            
            // 过滤异常值
            if (distance > 0 && distance < 400) {
                std::lock_guard<std::mutex> lock(distanceMutex);
                frontDistance = distance;
                
                // 基于距离控制小车状态
                if (distance < 20.0f) {
                    if (!paused.load()) {
                        std::cout << "\u26a0\ufe0f 前方障碍物距离过近: " << distance << " cm，暂停小车\n";
                        paused.store(true);
                    }
                } else {
                    if (paused.load()) {
                        std::cout << "\u2705 障碍物已清除，恢复运行\n";
                        paused.store(false);
                        pauseCV.notify_all();
                    }
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
};

void pauseIfNeeded(Motor& motor) {
    if (paused.load()) {
        motor.stop();
        std::unique_lock<std::mutex> lock(pauseMutex);
        pauseCV.wait(lock, [] { return !paused.load(); });
    }
}

int main() {
    MotorPins pins = { 17, 16, 22, 23 };
    Motor motor(pins);
    Servo servo(18);
    YawTracker yaw;
    
    UltrasonicSensor frontSensor("gpiochip0", 12, 4); // 前方传感器
    
    std::cout << "\U0001f680 小车开始运行，随时监测前方障碍物...\n";
    
    while (running.load()) {
        pauseIfNeeded(motor);
        motor.forward(80);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 缩短检查间隔
    }
    
    motor.stop();
    return 0;
}
