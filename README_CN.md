智能医院引导机器人（基于树莓派）

项目简介

本项目是一个智能医院引导机器人，基于树莓派开发，旨在帮助医院优化患者就诊流程。机器人具备面部识别、自动导航和避障能力，可根据患者挂号信息引导其前往相应科室，提高医院服务效率。

主要功能 
• 患者身份确认：通过摄像头进行面部识别，匹配医院数据库中的患者信息。 
• 自动导航：根据患者挂号信息，按照地图引导患者前往相应科室。 
• 避障功能：超声波传感器检测障碍物，并通过路径调整进行绕行。 
• 智能转向：使用陀螺仪检测方向偏差，保证行进稳定。 
• 语音交互：提供语音播报功能，提升患者体验。

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

# RoboHospitalGuide 项目目录结构

本项目采用模块化设计，遵循清晰的目录结构，便于维护和扩展。以下是项目的主要目录及其功能说明：

RoboHospitalGuide/
├── build/                # 构建输出目录（由CMake生成）
├── config/               # 项目配置文件
│   └── project_config.hpp # 项目级配置头文件
├── docs/                 # 文档目录
│   ├── api/              # API参考文档
│   └── design/           # 系统设计文档
├── include/              # 公共头文件
│   ├── core/             # 核心模块接口
│   │   ├── block.hpp       # 基础块抽象类
│   │   ├── navigation_block.hpp # 导航块接口
│   │   └── obstacle_block.hpp   # 障碍块接口
│   ├── drivers/          # 驱动层接口
│   │   ├── gyroscope_driver.hpp # 陀螺仪驱动
│   │   ├── motor_driver.hpp     # 电机驱动
│   │   ├── ultrasonic_driver.hpp # 超声波驱动
│   │   └── hal/           # 硬件抽象层
│   │       ├── gpio.hpp      # GPIO驱动
│   │       ├── i2c.hpp       # I2C驱动
│   │       ├── pwm.hpp       # PWM驱动
│   │       └── spi.hpp       # SPI驱动
│   └── utils/            # 工具类接口
│       ├── error_handling.hpp # 错误处理工具
│       └── logger.hpp       # 日志系统
├── scripts/              # 脚本目录（构建/部署脚本）
├── src/                  # 源文件实现
│   ├── core/             # 核心模块实现
│   │   ├── block.cpp       # 基础块实现
│   │   ├── navigation_block.cpp # 导航块实现
│   │   └── obstacle_block.cpp   # 障碍块实现
│   ├── drivers/          # 驱动层实现
│   │   ├── gyroscope_driver.cpp # 陀螺仪驱动实现
│   │   ├── motor_driver.cpp     # 电机驱动实现
│   │   └── hal/           # 硬件抽象层实现
│   │       ├── gpio.cpp      # GPIO驱动实现
│   │       ├── i2c.cpp       # I2C驱动实现
│   │       ├── pwm.cpp       # PWM驱动实现
│   │       └── spi.cpp       # SPI驱动实现
│   ├── utils/            # 工具类实现
│   │   └── logger.cpp       # 日志系统实现
│   └── main.cpp          # 主程序入口
├── tests/                # 单元测试目录
│   ├── core/             # 核心模块测试
│   │   └── test_block.cpp  # 基础块测试用例
│   ├── drivers/          # 驱动层测试
│   └── hal/              # 硬件抽象层测试
├── third_party/          # 第三方库目录
├── CMakeLists.txt        # CMake构建配置文件
└── README.md             # 项目说明文档

未来版本更新

版本	预期功能
v1.0	基础面部识别、路径规划、避障
v1.1	语音播报功能、简单语音交互
v1.2	远程控制（支持医生实时调度）
v2.0	深度学习优化导航、云端数据交互
