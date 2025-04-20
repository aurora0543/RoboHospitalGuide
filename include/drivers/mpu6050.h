#ifndef MPU6050_H
#define MPU6050_H

#include <cstdint>
#include <functional>
#include <atomic>
#include <thread>
#include <stdexcept>

class MPU6050 {
public:
    // Callback function type definition
    using DataCallback = std::function<void(uint64_t, const float[3], const float[3])>;

    // Constructor / Destructor
    MPU6050(const char* i2c_device = "/dev/i2c-1", uint8_t address = 0x68);
    ~MPU6050();

    // Public interface
    void registerCallback(DataCallback cb);
    void start(uint32_t interval_ms = 10);
    void stop();

private:
    // Private methods
    uint32_t interval_ms;
    void initializeSensor();
    void writeRegister(uint8_t reg, uint8_t value);
    void setAccelRange(uint8_t range);
    void setGyroRange(uint8_t range);
    void readThreadFunc();

    // Private member variables
    int file;                           // I2C file descriptor
    std::atomic<bool> running{false};   // Thread control flag
    std::thread read_thread;            // Sensor reading thread
    DataCallback callback;              // User-defined callback

    // Scale factors
    float accel_scale = 1.0f / 16384.0f;  // Default ±2g range
    float gyro_scale  = 1.0f / 131.0f;    // Default ±250 dps range

    // Register address constants (C++11 in-class initialization)
    enum : uint8_t {
        PWR_MGMT_1   = 0x6B,
        ACCEL_CONFIG = 0x1C,
        GYRO_CONFIG  = 0x1B,
        ACCEL_XOUT_H = 0x3B,
        SMPLRT_DIV   = 0x19
    };
};

#endif // MPU6050_H