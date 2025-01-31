# **Smart Hospital Guide Robot (Raspberry Pi-Based)**  

[中文](README_CN.md)  

## **Project Overview**  
This project is a smart hospital guide robot developed using Raspberry Pi. It aims to improve the efficiency of hospital services by helping patients navigate to the correct department based on their registration information. The robot features facial recognition, autonomous navigation, and obstacle avoidance to enhance the hospital experience.  

## **Key Features**  
- **Patient Identification**: Uses a camera to recognize patients and match them with the hospital database.  
- **Autonomous Navigation**: Calculates the optimal path to guide patients to their assigned department.  
- **Obstacle Avoidance**: Uses ultrasonic sensors to detect and avoid obstacles.  
- **Intelligent Steering**: Utilizes a gyroscope to ensure stable movement.  
- **Voice Interaction (Future Update)**: Provides voice guidance for a better user experience.  

## **Hardware Requirements**  
| **Component**         | **Purpose**                    |  
|----------------------|--------------------------------|  
| Raspberry Pi 4B     | Core computing module         |  
| Camera Module (Raspberry Pi Camera) | Facial recognition  |  
| Ultrasonic Sensor (HC-SR04) | Obstacle detection       |  
| Gyroscope (MPU6050) | Motion and direction sensing |  
| Motor Driver (L298N) | Controls the four-wheel movement |  
| 4WD Chassis        | Robot movement framework     |  
| Rechargeable Battery | Power supply                 |  

以下是 Project Structure (C++) 部分的 Markdown 格式修正版本：

## **Project Structure (C++)**  
```
├── Hospital-Guide-Robot/
│   ├── CMakeLists.txt  # CMake 配置文件
│   ├── src/
│   │   ├── main.cpp  # 主程序
│   │   ├── navigation.cpp  # 导航系统
│   │   ├── face_recognition.cpp  # 面部识别
│   │   ├── obstacle_avoidance.cpp  # 避障模块
│   │   ├── motor_control.cpp  # 电机控制
│   │   ├── sensors.cpp  # 传感器管理
│   │   ├── core/
│   │   │   ├── block.h  # Block 设计模式基类
│   │   │   ├── navigation_block.cpp  # 导航Block
│   │   │   ├── obstacle_block.cpp  # 避障Block
│   │   ├── drivers/
│   │   │   ├── ultrasonic_driver.cpp  # 超声波传感器驱动
│   │   │   ├── gyroscope_driver.cpp  # 陀螺仪驱动
│   │   │   ├── motor_driver.cpp  # 电机驱动
│   ├── include/
│   │   ├── navigation.h
│   │   ├── face_recognition.h
│   │   ├── obstacle_avoidance.h
│   │   ├── motor_control.h
│   │   ├── sensors.h
│   │   ├── core/
│   │   │   ├── block.h
│   │   ├── drivers/
│   │   │   ├── ultrasonic_driver.h
│   │   │   ├── gyroscope_driver.h
│   │   │   ├── motor_driver.h
│   ├── assets/  # 存放资源文件，如图像模型
│   ├── config/  # 配置文件，如路径参数
│   ├── README.md  # 项目文档

```

## **Installation & Execution**  
1. **Install Dependencies**:
    ```sh
    sudo apt update
    sudo apt install libopencv-dev # OpenCV for image processing
    sudo apt install wiringPi # GPIO control
    ```

2. **Clone Repository**:
    ```sh
    git clone https://github.com/your-repo/hospital-guide-robot.git
    cd hospital-guide-robot
    ```

3. **Compile and Run**:
    ```sh
    mkdir build && cd build
    cmake ..
    make
    ./hospital_guide_robot
    ```

## **Future Updates**  
| Version | Planned Features |  
|---------|-----------------|  
| v1.0    | Basic facial recognition, path planning, obstacle avoidance |  
| v1.1    | Voice guidance, basic voice interaction |  
| v1.2    | Remote control (allow doctors to manually adjust navigation) |  
| v2.0    | AI-enhanced navigation, cloud-based data interaction |  
