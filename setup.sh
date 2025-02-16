#!/bin/bash

# 获取当前目录
ROOT_DIR="$PWD"

# 目录结构
DIRECTORIES=(
    "doc"
    "include/core"
    "include/drivers"
    "include/drivers/hal"
    "src/core"
    "src/drivers"
    "src/drivers/hal"
)

# 需要创建的头文件
HEADER_FILES=(
    "include/core/block.h"
    "include/core/navigation_block.h"
    "include/core/obstacle_block.h"
    "include/drivers/gyroscope_driver.h"
    "include/drivers/motor_driver.h"
    "include/drivers/ultrasonic_driver.h"
    "include/drivers/face_recognition.h"
    "include/drivers/motor_control.h"
    "include/drivers/navigation.h"
    "include/drivers/obstacle_avoidance.h"
    "include/drivers/sensor.h"
    "include/drivers/hal/pwm.h"
    "include/drivers/hal/i2c.h"
    "include/drivers/hal/spi.h"
    "include/drivers/hal/gpio.h"
)

# 需要创建的源文件
SOURCE_FILES=(
    "src/core/block.c"
    "src/core/navigation_block.c"
    "src/core/obstacle_block.c"
    "src/drivers/gyroscope_driver.c"
    "src/drivers/motor_driver.c"
    "src/drivers/ultrasonic_driver.c"
    "src/drivers/hal/pwm.c"
    "src/drivers/hal/i2c.c"
    "src/drivers/hal/spi.c"
    "src/drivers/hal/gpio.c"
    "src/face_recognition.cpp"
    "src/main.cpp"
    "src/motor_control.cpp"
    "src/navigation.cpp"
    "src/obstacle_block.cpp"
    "src/sensors.cpp"
)

# 创建目录
echo "📂 创建缺失的目录..."
for dir in "${DIRECTORIES[@]}"; do
    if [ ! -d "$ROOT_DIR/$dir" ]; then
        mkdir -p "$ROOT_DIR/$dir"
        echo "✅ 目录创建: $dir"
    fi
done

# 生成头文件（如果不存在）
echo "📄 生成缺失的头文件..."
for file in "${HEADER_FILES[@]}"; do
    if [ ! -f "$ROOT_DIR/$file" ]; then
        cat <<EOF > "$ROOT_DIR/$file"
/**
 * @file $(basename $file)
 * @brief Auto-generated header file.
 */

#ifndef $(basename $file | tr a-z A-Z | sed 's/\./_/g')
#define $(basename $file | tr a-z A-Z | sed 's/\./_/g')

#ifdef __cplusplus
extern "C" {
#endif

void init_$(basename $file .h)(void);

#ifdef __cplusplus
}
#endif

#endif /* $(basename $file | tr a-z A-Z | sed 's/\./_/g') */
EOF
        echo "✅ 头文件创建: $file"
    fi
done

# 生成源文件（如果不存在）
echo "📄 生成缺失的源文件..."
for file in "${SOURCE_FILES[@]}"; do
    if [ ! -f "$ROOT_DIR/$file" ]; then
        cat <<EOF > "$ROOT_DIR/$file"
/**
 * @file $(basename $file)
 * @brief Auto-generated source file.
 */

#include "$(basename $file .c).h"

void init_$(basename $file .c)(void) {
    // TODO: Implement this function
}
EOF
        echo "✅ 源文件创建: $file"
    fi
done

# 确保 CMakeLists.txt 存在
if [ ! -f "$ROOT_DIR/CMakeLists.txt" ]; then
    echo "📄 创建 CMakeLists.txt..."
    cat <<EOF > "$ROOT_DIR/CMakeLists.txt"
cmake_minimum_required(VERSION 3.10)
project(RoboHospitalGuide)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_executable(RoboHospitalGuide
    src/main.cpp
    src/core/block.c
    src/core/navigation_block.c
    src/core/obstacle_block.c
    src/drivers/gyroscope_driver.c
    src/drivers/motor_driver.c
    src/drivers/ultrasonic_driver.c
    src/drivers/hal/pwm.c
    src/drivers/hal/i2c.c
    src/drivers/hal/spi.c
    src/drivers/hal/gpio.c
    src/face_recognition.cpp
    src/motor_control.cpp
    src/navigation.cpp
    src/obstacle_block.cpp
    src/sensors.cpp
)
EOF
    echo "✅ CMakeLists.txt 创建完成"
fi

# 确保 README.md 存在
if [ ! -f "$ROOT_DIR/README.md" ]; then
    echo "📄 创建 README.md..."
    cat <<EOF > "$ROOT_DIR/README.md"
# RoboHospitalGuide

## 项目介绍
本项目是一个用于医院引导机器人的嵌入式控制软件，基于 Raspberry Pi 进行开发。

## 目录结构
\`\`\`
$(tree "$ROOT_DIR")
\`\`\`

## 编译 & 运行
\`\`\`sh
mkdir build && cd build
cmake ..
make
./RoboHospitalGuide
\`\`\`
EOF
    echo "✅ README.md 创建完成"
fi

echo "🎉 项目初始化/更新完成！"