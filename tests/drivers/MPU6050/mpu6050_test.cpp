// main.cpp
#include <iomanip>
#include "MPU6050.h"
#include <iostream>
#include <csignal>
#include <unistd.h>

// 全局标志用于优雅退出
std::atomic<bool> exit_flag(false);

// 信号处理函数
void signal_handler(int signum) {
    std::cout << "\nReceived exit signal, shutting down..." << std::endl;
    exit_flag.store(true);
}

int main() {
    // 注册信号处理
    signal(SIGINT, signal_handler);
    
    try {
        MPU6050 sensor;
        sensor.registerCallback([](uint64_t timestamp, 
                                  const float* accel, 
                                  const float* gyro) {
            static uint32_t counter = 0;
            
            if(++counter % 10 == 0) {
                std::cout << "\nTimestamp: " << timestamp << "ms"
                          << "\nAccel: "
                          << std::fixed << std::setprecision(2)
                          << "X:" << std::setw(6) << accel[0] << " g, "
                          << "Y:" << std::setw(6) << accel[1] << " g, "
                          << "Z:" << std::setw(6) << accel[2] << " g"
                          << "\nGyro:  "
                          << "X:" << std::setw(6) << gyro[0] << " °/s, "
                          << "Y:" << std::setw(6) << gyro[1] << " °/s, "
                          << "Z:" << std::setw(6) << gyro[2] << " °/s"
                          << std::endl;
                counter = 0;
            }
        });
        
        // 启动数据采集（50Hz）
        std::cout << "Starting data acquisition at 50Hz..." << std::endl;
        sensor.start(20);
        
        // 主循环
        while(!exit_flag) {
            sleep(1);
        }
        
        // 停止采集
        std::cout << "Stopping sensor..." << std::endl;
        sensor.stop();
        
    } catch(const std::exception& e) {
        std::cerr << "\n!!! System Error: " << e.what() << std::endl;
        std::cerr << "Troubleshooting:" << std::endl;
        std::cerr << "1. Check I2C connection (i2cdetect -y 1)" << std::endl;
        std::cerr << "2. Verify sensor address" << std::endl;
        std::cerr << "3. Run with sudo if permission denied" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Test completed successfully." << std::endl;
    return EXIT_SUCCESS;
}