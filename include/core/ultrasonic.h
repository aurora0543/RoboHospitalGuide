#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <wiringPi.h>
#include <chrono>

// 超声波传感器接口类，用于测量距离
class Ultrasonic {
public:
    // 构造：指定触发和接收引脚
    Ultrasonic(int trigPin, int echoPin) : trigPin(trigPin), echoPin(echoPin) {
        // 初始化引脚模式
        wiringPiSetupGpio();
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
        digitalWrite(trigPin, LOW);
    }

    // 读取当前距离，单位：cm
    float readDistanceCm() {
        using namespace std::chrono;
        // 发出 10 微秒触发信号
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // 等待回声信号开始
        while (digitalRead(echoPin) == LOW);
        auto start = high_resolution_clock::now();
        // 等待回声信号结束
        while (digitalRead(echoPin) == HIGH);
        auto end = high_resolution_clock::now();

        // 计算时间差
        float duration_us = duration_cast<microseconds>(end - start).count();
        // 声速约 343m/s，往返距离
        float distance_cm = duration_us * 0.0343f / 2.0f;
        return distance_cm;
    }

private:
    int trigPin;
    int echoPin;
};

// 超声波传感器硬件接口定义
#define TRIG_FRONT 19
#define ECHO_FRONT 13
#define TRIG_LEFT   6
#define ECHO_LEFT   5
#define TRIG_RIGHT 24
#define ECHO_RIGHT 25
#define TRIG_REAR  12
#define ECHO_REAR   4

#endif // ULTRASONIC_H
