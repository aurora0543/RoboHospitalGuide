// MPU6050.h
#ifndef MPU6050_HPP
#define MPU6050_HPP

#include <cstdint>
#include <functional>
#include <atomic>
#include <thread>
#include <stdexcept>

class MPU6050 {
public:
    // 回调函数类型定义
    using DataCallback = std::function<void(uint64_t, const float[3], const float[3])>;

    // 构造函数/析构函数
    MPU6050(const char* i2c_device = "/dev/i2c-1", uint8_t address = 0x68);
    ~MPU6050();

    // 公开接口
    void registerCallback(DataCallback cb);
    void start(uint32_t interval_ms = 10);
    void stop();

private:
    // 私有成员函数
    uint32_t interval_ms;
    void initializeSensor();
    void writeRegister(uint8_t reg, uint8_t value);
    void setAccelRange(uint8_t range);
    void setGyroRange(uint8_t range);
    void readThreadFunc();

    // 私有成员变量
    int file;                           // I2C文件描述符
    std::atomic<bool> running{false};   // 线程控制标志
    std::thread read_thread;            // 数据读取线程
    DataCallback callback;              // 用户回调函数

    // 量程相关参数
    float accel_scale = 1.0f / 16384.0f;  // 默认±2g量程
    float gyro_scale  = 1.0f / 131.0f;    // 默认±250dps量程

    // 寄存器地址常量（C++11类内初始化）
    enum : uint8_t {
        PWR_MGMT_1   = 0x6B,
        ACCEL_CONFIG = 0x1C,
        GYRO_CONFIG  = 0x1B,
        ACCEL_XOUT_H = 0x3B,
        SMPLRT_DIV   = 0x19
    };
};

#endif // MPU6050_HPP