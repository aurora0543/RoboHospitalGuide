#pragma once

namespace RoboHospitalGuide {
namespace core {

class navigation_block {
public:
    explicit navigation_block(int id);
    virtual ~navigation_block() = default;

    // 删除拷贝构造和赋值
    navigation_block(const navigation_block&) = delete;
    navigation_block& operator=(const navigation_block&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace core
} // namespace RoboHospitalGuide
