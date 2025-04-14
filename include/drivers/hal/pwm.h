#ifndef PWM_H
#define PWM_H

#include <atomic>
#include <gpiod.h>

const int PWM_FREQUENCY = 1000;      // 1 kHz
const int PWM_RESOLUTION = 100;      // 0-100 占空比

class Motor;

#endif