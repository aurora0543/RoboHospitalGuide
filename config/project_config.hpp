#pragma once

#include <string>

namespace RoboHospitalGuide {
namespace config {

class project_config {
public:
    explicit project_config(int id);
    virtual ~project_config() = default;

    // 删除拷贝构造和赋值
    project_config(const project_config&) = delete;
    project_config& operator=(const project_config&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace config
} // namespace RoboHospitalGuide
