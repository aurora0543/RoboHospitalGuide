#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <gpiod.hpp>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>

// 默认GPIO引脚定义
#define TRIG_PIN  24  // GPIO 24
#define ECHO_PIN  25  // GPIO 25

// 声明获取当前时间（微秒）的函数
long getMicrotime();

class UltrasonicSensor {
public:
    // 回调类型定义，返回整型值表示状态
    using Callback = std::function<void(int)>;
    
    // 构造函数
    UltrasonicSensor(const std::string& chip_name, 
                    int trig_pin, 
                    int echo_pin, 
                    Callback callback);
    
    // 析构函数
    ~UltrasonicSensor();

private:
    std::unique_ptr<gpiod::chip> chip;
    gpiod::line trig_line;
    gpiod::line echo_line;
    std::atomic<bool> running;
    std::thread sensor_thread;
    Callback callback;
    
    // 测量距离的私有方法
    void measureDistance();
};

#endif // ULTRASONIC_SENSOR_H
