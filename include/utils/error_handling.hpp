#pragma once

namespace RoboHospitalGuide {
namespace utils {

class error_handling {
public:
    explicit error_handling(int id);
    virtual ~error_handling() = default;

    // 删除拷贝构造和赋值
    error_handling(const error_handling&) = delete;
    error_handling& operator=(const error_handling&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace utils
} // namespace RoboHospitalGuide
