cmake_minimum_required(VERSION 3.14)
project(MyCrowApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/crow/include
    /opt/homebrew/include
)

# 可执行文件
add_executable(web web.cpp)

target_link_libraries(web pthread)