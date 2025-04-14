#include "yaw_tracker.h"
#include <chrono>
#include "MadgwickAHRS.h"

// 全局实例（或作为成员变量）
Madgwick madgwick;

void YawTracker::start(int hz) {
    madgwick.begin(hz);  // 初始化采样频率
    mpu.registerCallback([this](uint64_t ts, const float* acc, const float* gyro) {
        this->handleMPUData(ts, acc, gyro);
    });
    mpu.start(hz);
}

void YawTracker::handleMPUData(uint64_t, const float* acc, const float* gyro) {
    madgwick.updateIMU(
        gyro[0], gyro[1], gyro[2],  // 单位：°/s
        acc[0], acc[1], acc[2]      // 单位：g
    );
}

float YawTracker::getAngle() const {
    return madgwick.getYaw(); // 获取融合后的 yaw（单位：°）
}

void YawTracker::reset() {
    // MadgwickAHRS 通常不带 reset，这里你可以重建对象或忽略
}