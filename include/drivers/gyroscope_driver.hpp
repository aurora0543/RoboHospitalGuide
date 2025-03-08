#pragma once

namespace RoboHospitalGuide {
namespace drivers {

class gyroscope_driver {
public:
    explicit gyroscope_driver(int id);
    virtual ~gyroscope_driver() = default;

    // 删除拷贝构造和赋值
    gyroscope_driver(const gyroscope_driver&) = delete;
    gyroscope_driver& operator=(const gyroscope_driver&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace drivers
} // namespace RoboHospitalGuide
