#include "ultrasonic_sensor.h"
#include <iostream>
#include <chrono>
#include <sys/time.h>

// 获取当前时间（微秒）
long getMicrotime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

UltrasonicSensor::UltrasonicSensor(const std::string& chip_name, 
                                 int trig_pin, 
                                 int echo_pin, 
                                 Callback callback)
    : running(true), callback(callback) {
    chip = std::make_unique<gpiod::chip>(chip_name);
    trig_line = chip->get_line(trig_pin);
    echo_line = chip->get_line(echo_pin);
    trig_line.request({"ultrasonic", gpiod::line_request::DIRECTION_OUTPUT, 0}, 0);
    echo_line.request({"ultrasonic", gpiod::line_request::DIRECTION_INPUT, 0});
    sensor_thread = std::thread(&UltrasonicSensor::measureDistance, this);
}

UltrasonicSensor::~UltrasonicSensor() {
    running = false;
    if (sensor_thread.joinable()) {
        sensor_thread.join();
    }
}

void UltrasonicSensor::measureDistance() {
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
        
        // 判断是否小于20cm，返回不同的值
        if (distance < 20.0f) {
            callback(0);  // 返回0表示暂停
        } else {
            callback(1);  // 返回1表示继续
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
