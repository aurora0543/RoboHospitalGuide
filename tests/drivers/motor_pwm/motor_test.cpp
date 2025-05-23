#include "motor.h"
#include "sovor.h" // 包含舵机控制库
#include "rpi_pwm.h"       // 包含 RPI_PWM 类的定义
#include <iostream>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

// 保存初始终端设置，以便退出时还原
struct termios orig_termios;

// 还原终端模式
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// 设置终端为原始模式，立即响应键盘事件，不需要回车，同时关闭回显
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode); // 程序退出时自动还原

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // 关闭回显和规范模式
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// 利用 select() 检测标准输入是否有键盘输入（非阻塞方式）
bool kbhit() {
    timeval tv = {0, 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    int ret = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &tv);
    return (ret > 0);
}

int main() {
    // 根据实际硬件接线配置定义电机控制引脚
    MotorPins pins = {
        17, // 左轮 AIN1
        16, // 左轮 AIN2
        22, // 右轮 BIN1
        23  // 右轮 BIN2
    };

    int servoChannel = 2;       // PWM 通道，根据你的硬件连接修改
    int servoFrequency = 50;     // PWM 频率 (Hz)
    RPI_PWM pwm;             // RPI_PWM 实例
    int currentAngle = 0;    // текущий угол поворота сервопривода

    try {
        Motor motor(pins, false);
        enableRawMode();  // 进入原始模式

        // 初始化舵机
        servo_init(pwm, servoChannel, servoFrequency);

        std::cout << "使用上下左右箭头控制汽车运动：" << std::endl;
        std::cout << "    上箭头: 前进 (PWM 50%)" << std::endl;
        std::cout << "    下箭头: 后退 (PWM 60%)" << std::endl;
        std::cout << "    左箭头: 左转 (舵机 30 度)" << std::endl;
        std::cout << "    右箭头: 右转 (舵机 30 度)" << std::endl;
        std::cout << "    q 键: 退出程序" << std::endl;

        bool running = true;
        while (running) {
            if (kbhit()) {
                char ch;
                // 读取一个字符
                read(STDIN_FILENO, &ch, 1);
                if (ch == '\x1b') {  // 检测到转义字符，可能为箭头键
                    char seq[2];
                    // 尝试读取后续两个字符
                    if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
                    if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;
                    if (seq[0] == '[') {
                        if (seq[1] == 'A') {  // 上箭头
                            std::cout << "前进" << std::endl;
                            motor.forwardRaw(35);
                        } else if (seq[1] == 'B') {  // 下箭头
                            std::cout << "后退" << std::endl;
                            motor.backwardRaw(35);
                        } else if (seq[1] == 'C') {  // 右箭头
                            std::cout << "右转" << std::endl;
                            currentAngle += 40;
                            if (currentAngle > 45) currentAngle = 45;
                            servo_turn(pwm, 'R', currentAngle); // 调用舵机右转函数
                            motor.stopRaw();           // 停止电机，根据需要调整
                           // std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 增加延时
                           // servo_center(pwm);
                        } else if (seq[1] == 'D') {  // 左箭头
                            std::cout << "左转" << std::endl;
                            currentAngle -= 40;
                            if (currentAngle < -45) currentAngle = -45;
                            servo_turn(pwm, 'L', std::abs(currentAngle)); // 调用舵机左转函数
                            motor.stopRaw();           // 停止电机，根据需要调整
                           // std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 增加延时
                           // servo_center(pwm);
                        }
                    }
                } else if (ch == 'q') {
                    std::cout << "退出程序" << std::endl;
                    running = false;
                }
            } else {
                // 没有检测到按键时，停止电机运动
                motor.stopRaw();
            }
            // 每 50 毫秒轮询一次
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // 停止电机并清理资源
        motor.stopRaw();
        motor.cleanup();
        pwm.stop(); // 停止 PWM 输出
    } catch (const std::exception &ex) {
        std::cerr << "错误: " << ex.what() << std::endl;
    }

    return 0;
}