智能医院引导机器人（基于树莓派）

项目简介

本项目是一个智能医院引导机器人，基于树莓派开发，旨在帮助医院优化患者就诊流程。机器人具备面部识别、自动导航和避障能力，可根据患者挂号信息引导其前往相应科室，提高医院服务效率。

主要功能
	•	患者身份确认：通过摄像头进行面部识别，匹配医院数据库中的患者信息。
	•	自动导航：根据患者挂号信息，计算最优路径，引导患者前往相应科室。
	•	避障功能：超声波传感器检测障碍物，并通过路径调整进行绕行。
	•	智能转向：使用陀螺仪检测方向偏差，保证行进稳定。
	•	语音交互（未来版本）：提供语音播报功能，提升患者体验。

所需硬件

组件	用途
树莓派 4B	处理核心
摄像头模块（Raspberry Pi Camera）	进行面部识别
超声波传感器（HC-SR04）	避障检测
陀螺仪（MPU6050）	监测方向变化
电机驱动模块（L298N）	控制四轮小车移动
轮式底盘（4WD）	机器人行进部分
充电锂电池组	供电

程序结构（C++）

/Hospital-Guide-Robot
│── src/
│   ├── main.cpp       # 主程序入口，初始化所有功能
│   ├── navigation.cpp # 导航模块，规划路径、调整方向
│   ├── face_recognition.cpp # 面部识别模块
│   ├── obstacle_avoidance.cpp # 避障逻辑
│   ├── motor_control.cpp # 控制电机驱动
│   ├── sensors.cpp    # 传感器数据处理（超声波、陀螺仪）
│── include/
│   ├── navigation.h
│   ├── face_recognition.h
│   ├── obstacle_avoidance.h
│   ├── motor_control.h
│   ├── sensors.h
│── assets/ # 存放模型、图片等资源文件
│── config/ # 配置文件，如路径规划参数
│── README.md
│── LICENSE

安装与运行
	1.	安装依赖：

sudo apt update
sudo apt install libopencv-dev # OpenCV 处理图像
sudo apt install wiringPi # 控制 GPIO


	2.	克隆仓库：

git clone https://github.com/your-repo/hospital-guide-robot.git
cd hospital-guide-robot


	3.	编译运行：

mkdir build && cd build
cmake ..
make
./hospital_guide_robot



未来版本更新

版本	预期功能
v1.0	基础面部识别、路径规划、避障
v1.1	语音播报功能、简单语音交互
v1.2	远程控制（支持医生实时调度）
v2.0	深度学习优化导航、云端数据交互
