// MPU6050.cpp
#include "mpu6050.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <chrono>
#include <iostream>
#include <stdexcept>

#define SMPLRT_DIV    0x19
#define CONFIG        0x1A
#define GYRO_CONFIG   0x1B
#define ACCEL_CONFIG  0x1C
#define PWR_MGMT_1    0x6B
#define ACCEL_XOUT_H  0x3B
#define GYRO_XOUT_H   0x43

MPU6050::MPU6050(const char* i2c_device, uint8_t address) 
    : running(false) {
    // open the IIC
    if((file = open(i2c_device, O_RDWR)) < 0) {
        throw std::runtime_error("Failed to open I2C device");
    }
    
    // set the I2C address
    if(ioctl(file, I2C_SLAVE, address) < 0) {
        close(file);
        throw std::runtime_error("Failed to set I2C address");
    }
    
    initializeSensor();
}

MPU6050::~MPU6050() {
    stop();
    if(file >= 0) {
        close(file);
    }
}

void MPU6050::initializeSensor() {
    try {
        // 设备复位
        writeRegister(PWR_MGMT_1, 0x80);
        usleep(100000);  // 等待复位完成
        
        // 唤醒设备并设置时钟源
        writeRegister(PWR_MGMT_1, 0x00);
        
        // 配置加速度计和陀螺仪
        writeRegister(ACCEL_CONFIG, 0x00);  // ±2g
        writeRegister(GYRO_CONFIG, 0x00);   // ±250dps
        
        // 设置采样率
        writeRegister(SMPLRT_DIV, 0x07);    // 1kHz/(1+7)=125Hz
        writeRegister(CONFIG, 0x00);        // 禁用DLPF
        
    } catch(const std::exception& e) {
        throw std::runtime_error(std::string("Sensor init failed: ") + e.what());
    }
}

void MPU6050::registerCallback(DataCallback cb) {
    callback = cb;
}

void MPU6050::start(uint32_t interval_ms) {
    if(!running.exchange(true)) {
        this->interval_ms = interval_ms; // 保存参数到成员变量
        read_thread = std::thread(&MPU6050::readThreadFunc, this);
    }
}

void MPU6050::stop() {
    if(running.exchange(false)) {
        if(read_thread.joinable()) {
            read_thread.join();
        }
    }
}

void MPU6050::readThreadFunc() {
    uint8_t buffer[14];
    uint8_t reg = ACCEL_XOUT_H;
    
    while(running) {
        try {
            // 获取时间戳
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            
            // 读取传感器数据
            if(write(file, &reg, 1) != 1) {
                throw std::runtime_error("I2C register write failed");
            }
            
            if(read(file, buffer, 14) != 14) {
                throw std::runtime_error("Incomplete data read");
            }
            
            // 解析加速度数据
            float accel[3];
            accel[0] = static_cast<int16_t>(buffer[0] << 8 | buffer[1]) / 16384.0f;
            accel[1] = static_cast<int16_t>(buffer[2] << 8 | buffer[3]) / 16384.0f;
            accel[2] = static_cast<int16_t>(buffer[4] << 8 | buffer[5]) / 16384.0f;
            
            // 解析陀螺仪数据
            float gyro[3];
            gyro[0] = static_cast<int16_t>(buffer[8] << 8  | buffer[9])  / 131.0f;
            gyro[1] = static_cast<int16_t>(buffer[10] << 8 | buffer[11]) / 131.0f;
            gyro[2] = static_cast<int16_t>(buffer[12] << 8 | buffer[13]) / 131.0f;
            
            // 触发回调函数
            if(callback) {
                callback(timestamp, accel, gyro);
            }
            
        } catch(const std::exception& e) {
            std::cerr << "Sensor Error: " << e.what() << std::endl;
            running = false;
        }
        
        // 维持采样间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(this->interval_ms));
    }
}

void MPU6050::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    if(write(file, buffer, 2) != 2) {
        throw std::runtime_error("Failed to write register");
    }
}