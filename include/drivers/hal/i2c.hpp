#pragma once

namespace RoboHospitalGuide {
namespace hal {

class i2c {
public:
    explicit i2c(int id);
    virtual ~i2c() = default;

    // 删除拷贝构造和赋值
    i2c(const i2c&) = delete;
    i2c& operator=(const i2c&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace hal
} // namespace RoboHospitalGuide
