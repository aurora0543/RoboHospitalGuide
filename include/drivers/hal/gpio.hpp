#pragma once

namespace RoboHospitalGuide {
namespace hal {

class gpio {
public:
    explicit gpio(int id);
    virtual ~gpio() = default;

    // 删除拷贝构造和赋值
    gpio(const gpio&) = delete;
    gpio& operator=(const gpio&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace hal
} // namespace RoboHospitalGuide
