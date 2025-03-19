cmake_minimum_required(VERSION 3.15)
project(RoboHospitalGuide)

# 全局配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 核心库
add_library(core_lib STATIC
    src/core/block.cpp
    src/core/nav.cpp
    src/core/obstacle_block.cpp
)

# 驱动库
add_library(driver_lib STATIC
    src/drivers/gyroscope_driver.cpp
    src/drivers/motor_driver.cpp
    src/drivers/hal/pwm.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/core
    ${PROJECT_SOURCE_DIR}/include/drivers
    ${PROJECT_SOURCE_DIR}/include/drivers/hal
    ${PROJECT_SOURCE_DIR}/include/utils
)

# 主程序
add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    core_lib
    driver_lib
)
