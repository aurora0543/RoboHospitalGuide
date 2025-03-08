#pragma once

namespace RoboHospitalGuide {
namespace hal {

class spi {
public:
    explicit spi(int id);
    virtual ~spi() = default;

    // 删除拷贝构造和赋值
    spi(const spi&) = delete;
    spi& operator=(const spi&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace hal
} // namespace RoboHospitalGuide
