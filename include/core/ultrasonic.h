#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <gpiod.hpp>
#include <functional>
#include <atomic>
#include <thread>
#include <string>

/**
 * @brief HC‑SR04 超声波传感器封装，自动在后台测距并通过回调报告
 */
class UltrasonicSensor {
public:
    using Callback = std::function<void(float)>;

    /**
     * @param chip_name GPIO chip 名称（如 "gpiochip0"）
     * @param trig_pin  触发引脚号（BCM 编号）
     * @param echo_pin  回波引脚号（BCM 编号）
     * @param callback  每次测量完成后调用，参数为测得的距离（cm）
     */
    UltrasonicSensor(const std::string& chip_name,
                     int trig_pin,
                     int echo_pin,
                     Callback callback);

    /// 停止测距并释放资源
    ~UltrasonicSensor();

    // 禁止拷贝
    UltrasonicSensor(const UltrasonicSensor&) = delete;
    UltrasonicSensor& operator=(const UltrasonicSensor&) = delete;

private:
    void measureDistance();  ///< 后台线程执行的测距循环

    std::unique_ptr<gpiod::chip> chip;
    gpiod::line              trig_line;
    gpiod::line              echo_line;

    std::atomic<bool> running;
    std::thread       sensor_thread;
    Callback          callback;


    // 超声波传感器硬件接口定义
    #define TRIG_FRONT 24
    #define ECHO_FRONT 25
    #define TRIG_LEFT  19
    #define ECHO_LEFT  13
    #define TRIG_RIGHT  6
    #define ECHO_RIGHT  5
    #define TRIG_REAR  12
    #define ECHO_REAR   4
};




#endif // ULTRASONIC_SENSOR_H

