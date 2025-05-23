cmake_minimum_required(VERSION 3.10)
project(RoboHospitalGuide)

# 编译设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/config)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")



# 查找依赖库
find_library(GPIOD_LIB gpiod REQUIRED)
# 查找 OpenCV
find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg SvgWidgets Sql Xml Network Multimedia MultimediaWidgets)
add_subdirectory(RobotGUI)

# 包含头文件路径
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/drivers
    ${CMAKE_SOURCE_DIR}/include/drivers/hal
    ${CMAKE_SOURCE_DIR}/include/core
)

# 收集源文件（不包含 tests/ 目录，因为它根本不在这里）
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/main.cpp"
)

# 构建可执行文件
add_executable(RoboHospitalGuide ${SRC_FILES})

target_include_directories(RoboHospitalGuide PRIVATE ${CONFIG_DIR} ${CMAKE_SOURCE_DIR}/RobotGUI)

# 链接库
target_link_libraries(RoboHospitalGuide
    ${GPIOD_LIB}
    ${OpenCV_LIBS}
    RobotGUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Sql
    Qt6::Xml
    Qt6::Network
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)