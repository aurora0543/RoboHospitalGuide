cmake_minimum_required(VERSION 3.15)
project(RoboHospitalGuide LANGUAGES CXX)

# 全局配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

# 核心库
add_library(core_lib STATIC
    src/core/block.cpp
    src/core/navigation_block.cpp
    src/core/obstacle_block.cpp
)

# 驱动库
add_library(driver_lib STATIC
    src/drivers/gyroscope_driver.cpp
    src/drivers/motor_driver.cpp
    src/drivers/hal/pwm.cpp
)

# 主程序
add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    core_lib
    driver_lib
)
