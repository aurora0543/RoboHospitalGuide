#ifndef NAV_H
#define NAV_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "motor.h"
#include "servo.h"
#include "yaw_tracker.h"
#include "json.hpp"  // nlohmann::json 的头文件

namespace Nav {

// 全局控制标志与同步机制
extern std::atomic<bool> startNavigation;
extern std::atomic<bool> pauseNavigation;
extern std::mutex navMutex;
extern std::condition_variable navCV;

// 暂停检查函数，暂停时停止电机运行
void checkPause(Motor& motor);

// 前进函数，参数 duration_ms 为前进时长（毫秒）
void moveForward(Motor& motor, int duration_ms);

// 左转函数，参数 angle 为转动角度（度）
void turnLeft(Motor& motor, Servo& servo, YawTracker& yaw, float angle);

// 右转函数，参数 angle 为转动角度（度）
void turnRight(Motor& motor, Servo& servo, YawTracker& yaw, float angle);

// 导航线程函数：轮询控制标志完成一次导航动作（前进、左转、前进、右转）
void navigationThread(Motor* motor, Servo* servo, YawTracker* yaw, const std::string& target, nlohmann::json navJson);

}  // namespace Nav

#endif  // NAV_H