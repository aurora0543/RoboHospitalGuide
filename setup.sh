#!/bin/bash
# RoboHospitalGuide项目初始化脚本（macOS兼容版）
# 最后更新：2023-10-15

#######################################
# 配置区（用户可自定义）
#######################################
ROOT_DIR="$PWD"
PROJECT_NAME="RoboHospitalGuide"
CMAKE_VERSION=3.15
CPP_STANDARD=17

# 目录结构（普通数组格式）
DIRECTORIES=(
    # Core模块
    "include/core"
    "src/core" 
    "tests/core"
    
    # 驱动层
    "include/drivers"
    "src/drivers"
    "tests/drivers"
    
    # HAL硬件抽象层
    "include/drivers/hal"
    "src/drivers/hal"
    
    # 工具类
    "include/utils"
    "src/utils"
    
    # 其他目录
    "config"
    "docs/design"
    "docs/api"
    "third_party"
    "scripts"
    "build"
)

# 头文件列表
HEADER_FILES=(
    # Core模块
    "include/core/block.hpp"
    "include/core/navigation_block.hpp"
    "include/core/obstacle_block.hpp"
    
    # 驱动层
    "include/drivers/gyroscope_driver.hpp"
    "include/drivers/motor_driver.hpp"
    "include/drivers/ultrasonic_driver.hpp"
    
    # HAL硬件抽象层
    "include/drivers/hal/pwm.hpp"
    "include/drivers/hal/i2c.hpp"
    "include/drivers/hal/spi.hpp"
    "include/drivers/hal/gpio.hpp"
    
    # 工具类
    "include/utils/logger.hpp"
    "include/utils/error_handling.hpp"
    
    # 配置
    "config/project_config.hpp"
)

# 源文件列表
SOURCE_FILES=(
    # Core实现
    "src/core/block.cpp"
    "src/core/navigation_block.cpp"
    "src/core/obstacle_block.cpp"
    
    # 驱动实现
    "src/drivers/gyroscope_driver.cpp"
    "src/drivers/motor_driver.cpp"
    "src/drivers/hal/pwm.cpp"
    
    # 工具类实现
    "src/utils/logger.cpp"
    
    # 主程序
    "src/main.cpp"
    
    # 测试文件
    "tests/core/test_block.cpp"
)

#######################################
# 目录创建函数
#######################################
create_directories() {
    echo "📂 创建项目目录结构..."
    
    for dir in "${DIRECTORIES[@]}"; do
        full_path="${ROOT_DIR}/${dir}"
        if [ ! -d "$full_path" ]; then
            mkdir -p "$full_path"
            echo "✅ 创建目录: $dir"
        fi
    done
}

#######################################
# 生成C++头文件
#######################################
generate_cpp_header() {
    local file_path="$1"
    local namespace=$(basename $(dirname "$file_path") | tr '/' '_')
    
    cat <<EOF > "$file_path"
#pragma once

namespace ${PROJECT_NAME//-/_} {
namespace ${namespace} {

class $(basename "${file_path%.*}") {
public:
    explicit $(basename "${file_path%.*}")(int id);
    virtual ~$(basename "${file_path%.*}")() = default;

    // 删除拷贝构造和赋值
    $(basename "${file_path%.*}")(const $(basename "${file_path%.*}")&) = delete;
    $(basename "${file_path%.*}")& operator=(const $(basename "${file_path%.*}")&) = delete;

    virtual void execute() = 0;

protected:
    int id_;
    std::string error_msg_;
};

} // namespace ${namespace}
} // namespace ${PROJECT_NAME//-/_}
EOF
}

#######################################
# macOS兼容的目录树生成
#######################################
generate_directory_tree() {
    if command -v tree &> /dev/null; then
        tree -d -L 3
    else
        echo "⚠️  建议安装tree命令：brew install tree"
        find . -type d -not -path '*/\.*' | sed -E 's/^\.//;s/[^-][^\/]*\//--/g;s/^/  /'
    fi
}

#######################################
# 主程序流程
#######################################
main() {
    # 创建目录结构
    create_directories
    
    # 生成头文件
    echo "📄 生成C++头文件..."
    for header in "${HEADER_FILES[@]}"; do
        if [ ! -f "${ROOT_DIR}/${header}" ]; then
            generate_cpp_header "${ROOT_DIR}/${header}"
            echo "✅ 生成头文件: $header"
        fi
    done
    
    # 生成源文件
    echo "📄 生成C++源文件..."
    for source in "${SOURCE_FILES[@]}"; do
        if [ ! -f "${ROOT_DIR}/${source}" ]; then
            class_name=$(basename "${source%.*}")
            cat <<EOF > "${ROOT_DIR}/${source}"
#include "${class_name}.hpp"

namespace ${PROJECT_NAME//-/_} {
namespace $(basename $(dirname "$source") | tr '/' '_') {

${class_name}::${class_name}(int id) : id_(id) {}

} // namespace $(basename $(dirname "$source") | tr '/' '_')
} // namespace ${PROJECT_NAME//-/_}
EOF
            echo "✅ 生成源文件: $source"
        fi
    done
    
    # 生成CMakeLists.txt
    if [ ! -f "${ROOT_DIR}/CMakeLists.txt" ]; then
        cat <<EOF > "${ROOT_DIR}/CMakeLists.txt"
cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(${PROJECT_NAME} LANGUAGES CXX)

# 全局配置
set(CMAKE_CXX_STANDARD ${CPP_STANDARD})
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
add_executable(\${PROJECT_NAME} 
    src/main.cpp
)

target_link_libraries(\${PROJECT_NAME} PRIVATE 
    core_lib
    driver_lib
)
EOF
        echo "✅ 生成CMakeLists.txt"
    fi
    
    # 生成README
    if [ ! -f "${ROOT_DIR}/README.md" ]; then
        cat <<EOF > "${ROOT_DIR}/README.md"
# ${PROJECT_NAME}

## 项目结构
\`\`\`
$(generate_directory_tree)
\`\`\`

## 构建说明
\`\`\`bash
mkdir -p build && cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make -j\$(sysctl -n hw.logicalcpu)
\`\`\`

## macOS依赖安装
\`\`\`bash
# 安装基础工具链
brew install cmake tree

# 安装开发依赖（可选）
brew install cppcheck llvm
\`\`\`
EOF
        echo "✅ 生成README.md"
    fi

    echo "🎉 项目初始化完成！"
    echo "👉 后续步骤："
    echo "   cd build && cmake .. && make"
}

main "$@"