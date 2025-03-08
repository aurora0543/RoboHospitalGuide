#pragma once

namespace RoboHospitalGuide {
namespace core {

class obstacle_block {
public:
    explicit obstacle_block(int id);
    virtual ~obstacle_block() = default;

    // 删除拷贝构造和赋值
    obstacle_block(const obstacle_block&) = delete;
    obstacle_block& operator=(const obstacle_block&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace core
} // namespace RoboHospitalGuide
