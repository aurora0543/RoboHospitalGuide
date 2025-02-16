cmake_minimum_required(VERSION 3.10)

# 项目名称
project(RoboHospitalGuide)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# 设置编译选项（优化、警告）
add_compile_options(-Wall -Wextra -Wpedantic -O2)

# 设定 include 目录
include_directories(include)

# 查找所有源文件
file(GLOB_RECURSE SOURCES
    "src/core/*.c"
    "src/drivers/*.c"
    "src/drivers/hal/*.c"
    "src/*.cpp"
)

# 生成可执行文件
add_executable(RoboHospitalGuide ${SOURCES})

# 链接必要的库（如 I2C、GPIO、WiringPi 等）
target_link_libraries(RoboHospitalGuide
    pthread         # 线程库
    m              # 数学库
    wiringPi       # Raspberry Pi GPIO 控制库
    rt             # 实时库 (real-time library)
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(RoboHospitalGuide ${OpenCV_LIBS})

# 打印信息
message(STATUS "📦 Building RoboHospitalGuide with ${CMAKE_CXX_COMPILER}")
message(STATUS "📂 Source files: ${SOURCES}")