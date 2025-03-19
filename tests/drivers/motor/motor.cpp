#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <thread>
#include <string>

#define GPIO_BASE "/sys/class/gpio"
#define PWM_BASE "/sys/class/pwm/pwmchip0"

// **电机 GPIO 引脚**
#define AIN1 17
#define AIN2 18
#define BIN1 22
#define BIN2 23

// **编码器 GPIO 引脚**
#define E1A 20
#define E1B 21
#define E2A 26
#define E2B 27

volatile int spin_count1 = 0;
volatile int spin_count2 = 0;

// **GPIO 操作函数**
void gpio_export(int pin) {
    std::ofstream file(GPIO_BASE "/export");
    file << pin;
}

void gpio_unexport(int pin) {
    std::ofstream file(GPIO_BASE "/unexport");
    file << pin;
}

void gpio_set_direction(int pin, const std::string &dir) {
    std::ofstream file(std::string(GPIO_BASE) + "/gpio" + std::to_string(pin) + "/direction");
    file << dir;
}

void gpio_set_edge(int pin, const std::string &edge) {
    std::ofstream file(std::string(GPIO_BASE) + "/gpio" + std::to_string(pin) + "/edge");
    file << edge;
}

void gpio_write(int pin, int value) {
    std::ofstream file(std::string(GPIO_BASE) + "/gpio" + std::to_string(pin) + "/value");
    file << value;
}

// **PWM 相关函数**
void pwm_export(int pwm_num) {
    std::ofstream file(PWM_BASE "/export");
    file << pwm_num;
}

void pwm_unexport(int pwm_num) {
    std::ofstream file(PWM_BASE "/unexport");
    file << pwm_num;
}

void pwm_set(int pwm_num, int period, int duty_cycle, bool enable) {
    std::string path = std::string(PWM_BASE) + "/pwm" + std::to_string(pwm_num);
    std::ofstream file;

    file.open(path + "/period");
    file << period;
    file.close();

    file.open(path + "/duty_cycle");
    file << duty_cycle;
    file.close();

    file.open(path + "/enable");
    file << (enable ? "1" : "0");
    file.close();
}

// **电机控制**
void motor_control(int speed_left, int speed_right) {
    int period = 1000000; // 1kHz 频率
    int duty_left = abs(speed_left) * period / 100;
    int duty_right = abs(speed_right) * period / 100;

    // 控制左轮方向
    if (speed_left > 0) { // 正转
        gpio_write(AIN1, 1);
        gpio_write(AIN2, 0);
    } else if (speed_left < 0) { // 反转
        gpio_write(AIN1, 0);
        gpio_write(AIN2, 1);
    } else { // 停止
        gpio_write(AIN1, 0);
        gpio_write(AIN2, 0);
    }

    // 控制右轮方向
    if (speed_right > 0) { // 正转
        gpio_write(BIN1, 1);
        gpio_write(BIN2, 0);
    } else if (speed_right < 0) { // 反转
        gpio_write(BIN1, 0);
        gpio_write(BIN2, 1);
    } else { // 停止
        gpio_write(BIN1, 0);
        gpio_write(BIN2, 0);
    }

    // 设置 PWM 占空比
    pwm_set(0, period, duty_left, true);
    pwm_set(1, period, duty_right, true);
}

// **运动控制**
void forward(int time_sleep) {
    std::cout << "小车前进 " << time_sleep << " 秒" << std::endl;
    motor_control(50, 50);
    sleep(time_sleep);
}

void stop() {
    std::cout << "小车停止" << std::endl;
    motor_control(0, 0);
}

// **编码器回调线程**
void encoder_callback(int gpio_pin, volatile int &spin_count) {
    std::string gpio_path = std::string(GPIO_BASE) + "/gpio" + std::to_string(gpio_pin) + "/value";
    int fd = open(gpio_path.c_str(), O_RDONLY);
    
    if (fd < 0) {
        std::cerr << "无法打开编码器 GPIO " << gpio_pin << std::endl;
        return;
    }

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI | POLLERR;

    char buffer[8];

    while (true) {
        lseek(fd, 0, SEEK_SET);
        read(fd, buffer, sizeof(buffer));

        if (poll(&pfd, 1, -1) > 0) {
            lseek(fd, 0, SEEK_SET);
            read(fd, buffer, sizeof(buffer));
            spin_count++;
            std::cout << "编码器 " << gpio_pin << " 计数: " << spin_count << std::endl;
        }
    }

    close(fd);
}

// **初始化**
void setup() {
    // 导出 GPIO
    gpio_export(AIN1); gpio_export(AIN2);
    gpio_export(BIN1); gpio_export(BIN2);
    gpio_export(E1A); gpio_export(E1B);
    gpio_export(E2A); gpio_export(E2B);

    // 设置 GPIO 方向
    gpio_set_direction(AIN1, "out");
    gpio_set_direction(AIN2, "out");
    gpio_set_direction(BIN1, "out");
    gpio_set_direction(BIN2, "out");

    gpio_set_direction(E1A, "in");
    gpio_set_direction(E1B, "in");
    gpio_set_direction(E2A, "in");
    gpio_set_direction(E2B, "in");

    // 设置编码器触发模式（上升沿和下降沿）
    gpio_set_edge(E1A, "both");
    gpio_set_edge(E2A, "both");

    // 导出 PWM
    pwm_export(0);
    pwm_export(1);
}

// **主函数**
int main() {
    setup();

    std::thread enc1(encoder_callback, E1A, std::ref(spin_count1));
    std::thread enc2(encoder_callback, E2A, std::ref(spin_count2));

    forward(5);
    stop();

    enc1.join();
    enc2.join();

    return 0;
}
