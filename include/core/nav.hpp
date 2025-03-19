#pragma once
#include <functional>
#include <string>
#include <memory>

namespace RoboHospitalGuide {
namespace core {

// 回调函数类型定义
using ReadInfraredCallback = std::function<std::string()>;
using ReadUltrasonicCallback = std::function<double()>;
using MoveCallback = std::function<void(double distance)>;
using RotateCallback = std::function<void(double degrees)>;

// 配置参数结构体
struct NavConfig {
    std::string map_path;
    ReadInfraredCallback read_infrared;
    ReadUltrasonicCallback read_ultrasonic;
    MoveCallback move_forward;
    RotateCallback rotate;
};

// 导航核心类
class nav {
public:
    explicit nav(int id);  // 构造函数
    void navigate_to(const std::string& target);
    void emergency_stop();

private:
    int id_;  // 机器人唯一标识
    class Impl;  // PIMPL模式隐藏实现
    std::unique_ptr<Impl> impl_;
};

} // namespace core
} // namespace RoboHospitalGuide