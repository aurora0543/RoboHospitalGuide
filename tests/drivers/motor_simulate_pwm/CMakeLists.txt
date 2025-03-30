cmake_minimum_required(VERSION 3.10)

# 项目信息
project(MotorControl VERSION 1.0 LANGUAGES CXX)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加可执行文件
add_executable(motor motor.cpp)

# 链接 gpiod 库
find_library(GPIOD_LIB gpiod REQUIRED)
if (GPIOD_LIB)
    target_link_libraries(motor PRIVATE ${GPIOD_LIB})
else()
    message(FATAL_ERROR "gpiod library not found")
endif()
