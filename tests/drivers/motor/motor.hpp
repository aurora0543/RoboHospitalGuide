#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <wiringPi.h>

class MotorControl {
public:
    MotorControl(int pin1, int pin2);
    void setDirection(bool forward);

private:
    int motorPin1;
    int motorPin2;
};

#endif // MOTOR_HPP