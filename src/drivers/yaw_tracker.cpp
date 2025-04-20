#include "yaw_tracker.h"
#include <chrono>
#include "MadgwickAHRS.h"

// Global instance (or use as a member variable)
Madgwick madgwick;

void YawTracker::start(int hz) {
    madgwick.begin(hz);  // Initialize sample rate
    mpu.registerCallback([this](uint64_t ts, const float* acc, const float* gyro) {
        this->handleMPUData(ts, acc, gyro);
    });
    mpu.start(hz);
}

void YawTracker::handleMPUData(uint64_t, const float* acc, const float* gyro) {
    madgwick.updateIMU(
        gyro[0], gyro[1], gyro[2],  // Units: degrees per second
        acc[0], acc[1], acc[2]      // Units: g (gravitational acceleration)
    );
}

float YawTracker::getAngle() const {
    return madgwick.getYaw(); // Return fused yaw angle (in degrees)
}

void YawTracker::reset() {
}