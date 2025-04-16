cmake_minimum_required(VERSION 3.10)
project(FaceRecognition)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

# 手动链接 PNG 所需库
find_package(PNG REQUIRED)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(face face.cpp)

target_link_libraries(face ${OpenCV_LIBS} dlib ${PNG_LIBRARIES} jpeg z webp)