#include "pid.h"

PIDController::PIDController(double kp, double ki, double kd)
    : kp_(kp), ki_(ki), kd_(kd), targetSpeed_(0), integral_(0), previousError_(0) {}

void PIDController::setTarget(double target) {
    targetSpeed_ = target;
}

double PIDController::compute(double currentSpeed, double deltaTime) {
    double error = targetSpeed_ - currentSpeed;
    integral_ += error * deltaTime;
    double derivative = (error - previousError_) / deltaTime;
    previousError_ = error;
    return kp_ * error + ki_ * integral_ + kd_ * derivative;
}
