#include "nav.h"  // 包含导航相关声明（动作函数、控制标志）
#include <iostream>      // 控制台 I/O
#include <thread>        // 多线程与 sleep
#include <chrono>        // 时间度量
#include <cmath>         // 数学函数
#include "ultrasonic.h" // 超声波传感器接口，用于检测障碍

using namespace std::chrono;  // 简化时钟调用

static constexpr float SAFE_DISTANCE = 20.0f;     // 安全距离阈值
static constexpr float EXTRA_CLEARANCE = 10.0f;   // 额外清除距离

// 模块内全局超声波传感器对象，trig/echo 在 ultrasonic.h 中定义
Ultrasonic sensorFront(TRIG_FRONT, ECHO_FRONT);
Ultrasonic sensorLeft (TRIG_LEFT,  ECHO_LEFT);
Ultrasonic sensorRight(TRIG_RIGHT, ECHO_RIGHT);
Ultrasonic sensorRear (TRIG_REAR,  ECHO_REAR);

// 全局位置和航向，用于跨阶段累计
struct Position { float x=0, y=0; };
static Position currentPosition;   // 当前坐标（cm）

// 更新位置函数（单位 cm）
void updatePosition(float dist, float heading) {
    float rad = heading * M_PI / 180.0f;
    currentPosition.x += dist * std::cos(rad);
    currentPosition.y += dist * std::sin(rad);
    std::cout << "[定位] 移动 " << dist << " cm, 当前位置 (" \
              << currentPosition.x << ", " << currentPosition.y \
              << "), 航向 " << heading << "°\n";
}

// 匿名命名空间，用于内部函数实现
namespace {
    // 检测前方障碍
    bool isObstacleAhead() {
        float d = sensorFront.read();            // 读取前方距离
        std::cout << "[检测] 前方距离 = " << d << " cm\n";
        return d < SAFE_DISTANCE;                // 小于阈值即为障碍
    }
}

namespace Nav {

    extern void avoidObstacle(Motor*, Servo*, YawTracker*, float& heading, float& remaining, float& lateralComp);

    void navigationThread(Motor* motor, Servo* servo, YawTracker* yaw, nlohmann::json navJson) {
        Motor& m = *motor;
        Servo& s = *servo;
        YawTracker& y = *yaw;

        float heading = 0.0f;      // 当前航向
        float lateralComp = 0.0f;  // 跨阶段侧向补偿（累计）

        while (true) {
            if (startNavigation.load()) {
                std::cout << "\n🚦 开始导航...\n";

                for (const auto& step : navJson["path"]) {
                    std::string action = step["action"];
                    float value = step["value"];

                    if (action == "moveForward") {
                        float remaining = value;  // 本阶段剩余距离
                        std::cout << "⬆️ 目标前进 " << remaining << " cm\n";
                        m.forward(30);
                        while (remaining > 0.0f) {
                            if (isObstacleAhead()) {
                                avoidObstacle(motor, servo, yaw, heading, remaining, lateralComp);
                                m.stop();
                                break;
                            }
                            std::this_thread::sleep_for(milliseconds(10));
                            remaining -= 1.0f;
                            updatePosition(1.0f, heading);
                        }
                        m.stop();
                        if (remaining > 0.0f) {
                            std::cout << "⬆️ 回到原航向后继续前进 " << remaining << " cm\n";
                            m.forward(30);
                            std::this_thread::sleep_for(milliseconds((int)(remaining * 10)));
                            updatePosition(remaining, heading);
                            m.stop();
                        }

                    } else if (action == "turnLeft") {
                        std::cout << "↪️ 左转 " << value << " 度...\n";
                        turnLeft(m, s, y, value);
                        heading = std::fmod(heading + value + 360.0f, 360.0f);
                        // ✅ 合并 lateralComp 到本阶段 value 中
                        if (lateralComp != 0.0f) {
                            float lateralHeading = (lateralComp > 0) ? std::fmod(heading - 90 + 360.0f, 360.0f)
                                                                          : std::fmod(heading + 90, 360.0f);
                            float headingDiff = std::fabs(std::fmod(lateralHeading - heading + 360.0f, 360.0f));
                            bool sameDirection = (headingDiff < 90.0f || headingDiff > 270.0f);

                            float adjusted = sameDirection ? (value - std::abs(lateralComp))
                                                          : (value + std::abs(lateralComp));

                            std::cout << "➡️ 已合并侧向补偿，调整前进距离为 " << adjusted << " cm
";
                            value = adjusted;
                            lateralComp = 0.0f;
                        }
}

                    } else if (action == "turnRight") {
                        std::cout << "↩️ 右转 " << value << " 度...\n";
                        turnRight(m, s, y, value);
                        heading = std::fmod(heading - value + 360.0f, 360.0f);
                        // ⛔️ 已合并到 moveForward 中，此处不再执行

                    } else {
                        std::cerr << "❌ 未知导航动作: " << action << "\n";
                    }
                }

                std::cout << "🏁 导航完成！\n";
                startNavigation.store(false);
            }
            std::this_thread::sleep_for(milliseconds(100));
        }
    }

    void avoidObstacle(Motor* motor, Servo* servo, YawTracker* yaw,
                       float& heading, float& remaining, float& lateralComp) {
        Motor& m = *motor;
        Servo& s = *servo;
        YawTracker& y = *yaw;

        float pending = remaining;
        std::cout << "[避障] 记录剩余距离: " << pending << " cm\n";

        float ld = sensorLeft.read();
        float rd = sensorRight.read();
        bool goLeft = (ld > rd);
        std::cout << "[避障] 选择" << (goLeft?"左侧":"右侧") << "避障\n";

        if (goLeft) { turnLeft(m, s, y, 90); heading = std::fmod(heading + 90 + 360.0f, 360.0f); }
        else       { turnRight(m,s, y, 90); heading = std::fmod(heading - 90 + 360.0f, 360.0f); }

        float lateral = 0.0f;
        while (true) {
            float sense = goLeft? sensorRight.read(): sensorLeft.read();
            if (sense >= SAFE_DISTANCE) break;
            m.forward(30);
            std::this_thread::sleep_for(milliseconds(100));
            lateral += 1.0f;
            updatePosition(1.0f, heading);
        }

        std::cout << "[避障] 侧边安全，额外前进 " << EXTRA_CLEARANCE << " cm\n";
        m.forward(30);
        std::this_thread::sleep_for(milliseconds((int)(EXTRA_CLEARANCE * 10)));
        lateral += EXTRA_CLEARANCE;
        updatePosition(EXTRA_CLEARANCE, heading);
        m.stop();

        lateralComp += (goLeft? lateral : -lateral);
        std::cout << "[避障] 累计侧向补偿: " << lateralComp << " cm\n";

        if (goLeft) { turnRight(m,s,y,90); heading = std::fmod(heading - 90 + 360.0f, 360.0f); }
        else        { turnLeft(m, s, y, 90); heading = std::fmod(heading + 90 + 360.0f, 360.0f); }

        remaining = pending;
        std::cout << "[避障] 恢复剩余前进: " << remaining << " cm\n";
    }

}  // namespace Nav
