#pragma once

namespace RoboHospitalGuide {
namespace core {

class block {
public:
    explicit block(int id);
    virtual ~block() = default;

    // 删除拷贝构造和赋值
    block(const block&) = delete;
    block& operator=(const block&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace core
} // namespace RoboHospitalGuide
