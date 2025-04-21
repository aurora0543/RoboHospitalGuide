# RoboHospitalGuide: Smart Hospital Guide Robot

![body](image/body.jpg)

[CHN Version](README_CN.md)  

[Process Identification](ProcessIdentification.md)

[Develop log](Develop_log.md)

## 🩺 Project Overview
RoboHospitalGuide is an intelligent hospital guide robot system developed on Raspberry Pi 5B. This system is designed to enhance the hospital experience by offering autonomous navigation, facial recognition, and patient guidance services. Integrated with a graphical user interface (GUI) and real-time hardware control, this project showcases a modular and scalable architecture suitable for real-world hospital deployments.

The robot navigates patients to their assigned departments using face recognition and hospital database lookup, while dynamically avoiding obstacles and playing guiding audio instructions. The system is structured with a multithreaded backend and a Qt6-based GUI frontend.

### 🧾 Development Diary

We documented the development process through short videos and updates on Douyin (TikTok China).

📹 Follow our journey on Douyin: **@baron0543**

You can scan the QR code below using the Douyin app to view our profile:

<p align="center">
  <img src="image/douyin.JPG" alt="Douyin QR Code" width="250"/>
</p>

The videos include:

- Module integration tests  
- Troubleshooting and debugging logs  
- Real-time demonstrations of system functions  
- Behind-the-scenes development clips

Stay tuned for more updates!

## 🎯 Key Features

### 🔐 Facial Recognition
- Uses OpenCV and a custom-trained recognizer to identify patients.
- Matches patient face data against preloaded database images.
- GUI provides live face detection feedback.

### 🧭 Autonomous Navigation
- Loads department destination from GUI input or recognition match.
- Navigates using predefined map paths loaded from JSON.
- Controls four-wheel drive and direction with GPIO and PWM.

### 🚧 Obstacle Avoidance
- Equipped with ultrasonic sensors to detect and bypass dynamic obstacles.
- Navigation pauses and resumes automatically based on sensor feedback.

### 🧠 Intelligent Motion Control
- Incorporates gyroscope (MPU6050) for motion stability.
- Real-time yaw correction for precise turns and path tracking.

### 🗣️ Audio Feedback
- Plays pre-recorded audio during navigation: start, hold, stop.
- Future support for voice interaction and dynamic audio response.

### 🖥️ Qt6 GUI Interface
- Modern UI built with Qt6 (Widgets + Designer).
- Allows manual patient input, command triggers, and status viewing.
- Displays recognition results, navigation status, and debug logs.

## 🧱 Hardware Requirements
| Component | Purpose |
|----------|---------|
| Raspberry Pi 5B | Main control board |
| Camera Module | Face recognition |
| Ultrasonic Sensors | Obstacle detection |
| Gyroscope (MPU6050) | Orientation and yaw feedback |
| Motor Driver (L298N) | Drive control for 4WD chassis |
| 4WD Robot Chassis | Robot mobility |
| Rechargeable Battery | Power source |
| Optional: Touchscreen | GUI display (or remote access via VNC) |

## 📎 GPIO/SPI/I2C Wiring Diagram

```
+---------------------+-------------------------+----------------+
| Device              | Raspberry Pi GPIO Pin   | Notes          |
+---------------------+-------------------------+----------------+
| Ultrasonic Trigger  | GPIO17 (Pin 11)         | Output         |
| Ultrasonic Echo     | GPIO27 (Pin 13)         | Input          |
| MPU6050 SCL         | GPIO3  (Pin 5 - I2C)    | I2C Clock      |
| MPU6050 SDA         | GPIO2  (Pin 3 - I2C)    | I2C Data       |
| Motor IN1           | GPIO16 (Pin 36)         | Motor L298N    |
| Motor IN2           | GPIO20 (Pin 38)         | Motor L298N    |
| Motor IN3           | GPIO21 (Pin 40)         | Motor L298N    |
| Motor IN4           | GPIO26 (Pin 37)         | Motor L298N    |
| Servo (Yaw)         | GPIO18 (Pin 12 - PWM)   | PWM Control    |
| Camera              | CSI Port                | Face Capture   |
+---------------------+-------------------------+----------------+
```

> ⚠️ Ensure I2C and SPI are enabled using `sudo raspi-config > Interface Options`.

## 🧱 Software Dependencies & Qt Configuration

### ✅ Required Libraries
- Qt6 Core, Widgets, SVG, Network, Sql, Xml
- OpenCV 4.x
- libgpiod (GPIO control)
- WiringPi or alternatives (optional)
- ALSA or MPlayer (for audio playback)

### ✅ Install Qt6 (using Qt Online Installer)
- Select "Embedded ARM 64-bit" or compile for desktop ARM64
- Include: Qt6 Core, Widgets, Designer, SvgWidgets, Sql, Network

### ✅ Install OpenCV
```bash
sudo apt install libopencv-dev libopencv-core-dev
```

### ✅ Database Setup
1.	Install MySQL or MariaDB (if not already installed).
2.	Create the Database
You can find SQL setup files in the config/ directory. Run the following SQL files in order to create the necessary database and tables:
```bash
mysql -u root -p < config/init_database.sql
mysql -u root -p < config/create_tables.sql
```
(Adjust file names according to what’s in your config/ folder.)

3.	Create a Database User
Run the following SQL command (or use the provided SQL script):

CREATE USER 'remoter_user'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON your_database_name.* TO 'remoter_user'@'localhost';
FLUSH PRIVILEGES;


4.	Configure Database Connection
Make sure your application’s config file (e.g. .env or config/db_config.h) is properly set to use:
	•	User: remoter_user
	•	Password: your_password
	•	Database: your_database_name

### ✅ Install libgpiod
```bash
sudo apt install libgpiod-dev gpiod
```

### ✅ Enable I2C and SPI
```bash
sudo raspi-config
# Enable Interface Options > I2C, SPI
```

### ✅ Testing

We have provided test programs for various hardware modules to ensure each component functions correctly before integration. You can find these test files in the tests/ or modules/test/ directory.

The following modules are covered:
-	Motor Control – Test forward, backward, and turning operations
-	Servo (PWM) Control – Verify angle positioning for servos
-	Ultrasonic Sensor – Measure distance and verify sensor readings
-	Gyroscope / IMU – Read orientation or acceleration data
-	Web Module – Test basic communication and remote control interface

To run a test, navigate to the corresponding folder and compile or execute the test file. Detailed instructions can be found in comments within each test source file.

## 🗂️ Project Structure
```
include/            # C++ headers
├── core/           # Core logic (face recognition, navigation)
├── drivers/        # Hardware abstraction
    └── hal/        # HAL (e.g., GPIO, motor, MPU)

src/                # C++ source files
├── core/
├── drivers/
│   └── hal/

RobotGUI/           # Qt6 GUI frontend (mainwindow, signals, slots)
config/             # Config files (nav.json, hospital_map.svg, etc.)
docs/               # Documentation

tests/              # Unit and integration tests
third_party/        # External libs (if any)
build/              # Build output folder (ignored in repo)
```

## 🚀 Installation & Running
### Prerequisites
- Raspberry Pi OS (64-bit)
- Qt 6.5+ installed (via Qt Online Installer)
- OpenCV 4.x installed
- CMake + g++ 10+
- GPIOD + ALSA

### Building the project
```bash
git clone https://github.com/aurora0543/RoboHospitalGuide.git
cd RoboHospitalGuide
git clone https://github.com/aurora0543/RobotGUI.git

# or you can use --recurse-submodules to clone the submodules 

git clone 
mkdir build && cd build
cmake ..
make -j4
```

### Configuration Notes
- Face Recognition Path Configuration:
Please note that the face recognition model path in the code is currently set using an absolute path. You will need to modify the path based on your local file system in order for the face detection to work properly.
Example:

```c++
std::string modelPath = "/home/user/project/models/haarcascade_frontalface_default.xml";
```

- Update this path to match your actual directory structure.
- Make sure the required model files (e.g., OpenCV Haar Cascade) are correctly downloaded and placed in an accessible folder.
- If you move the project to a different machine or directory, this configuration must be updated accordingly.

### Running
```bash
sudo ./RoboHospitalGuide
```

## 🧩 Future Roadmap
| Version | Planned Features |
|---------|------------------|
| v1.1 | Voice guidance, basic voice interaction |
| v1.2 | Remote control panel for doctor override |
| v2.0 | path planning, database integration |
| v2.1 | Patient queue management, multilingual support |

## 📜 License
MIT License (or your preferred license)

## 👨‍💻 Contributors
- [aurora0543](https://github.com/aurora0543)
- [EricZJ](https://github.com/323417)
- [zz117zz](https://github.com/zz117zz)
- [Wswwhcs4](https://github.com/Wswwhcs4) 

---
Feel free to open issues or submit pull requests for improvements!

