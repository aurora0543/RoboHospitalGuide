#pragma once

namespace RoboHospitalGuide {
namespace hal {

class pwm {
public:
    explicit pwm(int id);
    virtual ~pwm() = default;

    // 删除拷贝构造和赋值
    pwm(const pwm&) = delete;
    pwm& operator=(const pwm&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace hal
} // namespace RoboHospitalGuide
