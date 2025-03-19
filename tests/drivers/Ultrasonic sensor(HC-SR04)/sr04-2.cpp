#include <iostream>
#include <fstream>
#include <unistd.h>   // usleep()
#include <sys/time.h> // gettimeofday()
#include <thread>     // 多线程
#include <atomic>     // 线程安全变量
#include <csignal>    // 处理 Ctrl+C 退出

#define TRIG_PIN 24  // 修改后的 TRIG 引脚 (GPIO 24, Pin 18)
#define ECHO_PIN 25  // 修改后的 ECHO 引脚 (GPIO 25, Pin 22)

using namespace std;

// 线程安全变量，存储最新测距数据
atomic<double> latest_distance(0.0);
atomic<bool> running(true);  // 控制测距线程是否继续运行

// 写入 sysfs GPIO 文件的方法
void writeToFile(const string& path, const string& value) {
    ofstream file(path);
    if (file.is_open()) {
        file << value;
        file.close();
    } else {
        cerr << "Error: Cannot write to " << path << endl;
    }
}

// 读取 sysfs GPIO 文件的方法
string readFromFile(const string& path) {
    ifstream file(path);
    string value;
    if (file.is_open()) {
        file >> value;
        file.close();
    } else {
        cerr << "Error: Cannot read from " << path << endl;
    }
    return value;
}

// 获取当前时间（微秒）
long getMicrotime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

// 初始化 GPIO
void setup() {
    writeToFile("/sys/class/gpio/export", to_string(TRIG_PIN));
    writeToFile("/sys/class/gpio/export", to_string(ECHO_PIN));
    usleep(100000); // 等待 GPIO 配置生效

    writeToFile("/sys/class/gpio/gpio" + to_string(TRIG_PIN) + "/direction", "out");
    writeToFile("/sys/class/gpio/gpio" + to_string(ECHO_PIN) + "/direction", "in");

    writeToFile("/sys/class/gpio/gpio" + to_string(TRIG_PIN) + "/value", "0");
    usleep(500000); // 让传感器稳定
}

// 获取测距数据
float getDistance() {
    // 发送超声波脉冲
    writeToFile("/sys/class/gpio/gpio" + to_string(TRIG_PIN) + "/value", "1");
    usleep(10);
    writeToFile("/sys/class/gpio/gpio" + to_string(TRIG_PIN) + "/value", "0");

    // 等待 ECHO 变为 HIGH（开始时间）
    while (readFromFile("/sys/class/gpio/gpio" + to_string(ECHO_PIN) + "/value") == "0");
    long startTime = getMicrotime();

    // 等待 ECHO 变为 LOW（结束时间）
    while (readFromFile("/sys/class/gpio/gpio" + to_string(ECHO_PIN) + "/value") == "1");
    long travelTime = getMicrotime() - startTime;

    // 计算距离（单位 cm）
    float distance = travelTime * 0.0343 / 2;
    return distance;
}

// 释放 GPIO 资源
void cleanup() {
    writeToFile("/sys/class/gpio/unexport", to_string(TRIG_PIN));
    writeToFile("/sys/class/gpio/unexport", to_string(ECHO_PIN));
}

// 测距线程，持续更新 `latest_distance`
void measureDistance() {
    while (running) {
        latest_distance = getDistance();
        usleep(100000); // 每 100ms 更新一次
    }
}

// 处理 Ctrl+C 退出
void signalHandler(int signum) {
    cout << "\nStopping measurement..." << endl;
    running = false;
    this_thread::sleep_for(milliseconds(200)); // 确保测距线程退出
    cleanup();
    exit(signum);
}

int main() {
    setup();

    // 绑定 Ctrl+C 信号处理
    signal(SIGINT, signalHandler);

    // 启动测距线程
    thread distanceThread(measureDistance);

    // 主线程随时获取最新测距数据
    while (running) {
        cout << "Current Distance: " << latest_distance << " cm" << endl;
        usleep(500000); // 每 500ms 显示一次
    }

    // 等待测距线程结束
    distanceThread.join();
    return 0;
}
