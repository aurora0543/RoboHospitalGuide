#pragma once

namespace RoboHospitalGuide {
namespace drivers {

class motor_driver {
public:
    explicit motor_driver(int id);
    virtual ~motor_driver() = default;

    // 删除拷贝构造和赋值
    motor_driver(const motor_driver&) = delete;
    motor_driver& operator=(const motor_driver&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace drivers
} // namespace RoboHospitalGuide
