#ifndef NAV_H
#define NAV_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "motor.h"
#include "servo.h"
//#include "servonew.h" // Alternative to "servo.h"
#include "yaw_tracker.h"
#include "json.hpp"  // Header for nlohmann::json

namespace Nav {

// Global control flags and synchronization primitives
extern std::atomic<bool> startNavigation;
extern std::atomic<bool> pauseNavigation;
extern std::mutex navMutex;
extern std::condition_variable navCV;

// Checks if navigation is paused; stops motor while paused
void checkPause(Motor& motor);

// Moves forward for the specified duration in milliseconds
void moveForward(Motor& motor, int duration_ms);

// Turns left by a specified angle in degrees
void turnLeft(Motor& motor, Servo& servo, YawTracker& yaw, float angle);

// Turns right by a specified angle in degrees
void turnRight(Motor& motor, Servo& servo, YawTracker& yaw, float angle);

// Main navigation thread function: executes navigation steps (e.g. forward, turn left/right)
void navigationThread(Motor* motor, Servo* servo, YawTracker* yaw, const std::string& target, nlohmann::json navJson);

}  // namespace Nav

#endif  // NAV_H