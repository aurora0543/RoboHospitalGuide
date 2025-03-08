#pragma once

namespace RoboHospitalGuide {
namespace drivers {

class ultrasonic_driver {
public:
    explicit ultrasonic_driver(int id);
    virtual ~ultrasonic_driver() = default;

    // 删除拷贝构造和赋值
    ultrasonic_driver(const ultrasonic_driver&) = delete;
    ultrasonic_driver& operator=(const ultrasonic_driver&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace drivers
} // namespace RoboHospitalGuide
