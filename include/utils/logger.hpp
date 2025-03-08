#pragma once

namespace RoboHospitalGuide {
namespace utils {

class logger {
public:
    explicit logger(int id);
    virtual ~logger() = default;

    // 删除拷贝构造和赋值
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace utils
} // namespace RoboHospitalGuide
