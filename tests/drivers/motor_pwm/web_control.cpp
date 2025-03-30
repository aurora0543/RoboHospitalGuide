#include "motor.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <termios.h> // 用于终端控制
#include <unistd.h>  // 用于 read 函数

// 编译指令 (确保 wiringPi 已安装):
// g++ -o motor_control motor_control.cpp -l wiringPi

// 获取键盘输入的函数，非阻塞模式
int getch() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO); // 禁用规范模式和回显
    newattr.c_iflag &= ~(ICRNL | INLCR);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    newattr.c_cc[VMIN] = 0; // 最小读取字符数为 0
    newattr.c_cc[VTIME] = 0; // 读取超时时间为 0
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

int main() {
    // 根据实际硬件接线配置定义电机控制引脚
    MotorPins pins = {
        17, // 左轮 AIN1
        18, // 左轮 AIN2
        22, // 右轮 BIN1
        23  // 右轮 BIN2
    };

    try {
        Motor motor(pins, false); // 初始化电机控制对象，使用Raw模式

        std::cout << "使用 'w' 前进, 's' 后退, 'q' 退出." << std::endl;

        int ch;
        bool running = true;
        int speed = 50; // 默认速度

        while (running) {
            ch = getch(); // 使用自定义的非阻塞 getch()

            switch (ch) {
                case 'w':
                    std::cout << "前进，速度 " << speed << "%" << std::endl;
                    motor.forwardRaw(speed);
                    break;
                case 's':
                    std::cout << "后退，速度 " << speed << "%" << std::endl;
                    motor.backwardRaw(speed);
                    break;
                case 'q':
                    std::cout << "停止并退出." << std::endl;
                    motor.stopRaw();
                    running = false;
                    break;
                case ' ':
                    std::cout << "停止." << std::endl;
                    motor.stopRaw();
                    break;
                 case '+':
                    speed = std::min(100, speed + 10);
                    std::cout << "速度增加到 " << speed << "%" << std::endl;
                    break;
                case '-':
                    speed = std::max(0, speed - 10);
                    std::cout << "速度降低到 " << speed << "%" << std::endl;
                    break;
                default:
                    // 忽略其他字符
                    break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); //避免频繁读按键
        }
        motor.cleanup(); // 清理电机驱动器

    } catch (const std::exception& ex) {
        std::cerr << "错误: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
