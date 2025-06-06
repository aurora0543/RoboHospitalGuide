#ifndef YAW_TRACKER_H
#define YAW_TRACKER_H

#include "mpu6050.h"
#include "MadgwickAHRS.h"

class YawTracker {
public:
    void start(int hz = 50);
    float getAngle() const;
    void reset();

private:
    MPU6050 mpu;
    void handleMPUData(uint64_t, const float*, const float*);
};

#endif // YAW_TRACKER_H