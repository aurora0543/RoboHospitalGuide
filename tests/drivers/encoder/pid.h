#ifndef PID_H
#define PID_H

class PIDController {
public:
    PIDController(double kp, double ki, double kd);
    void setTarget(double target);
    double compute(double currentSpeed, double deltaTime);

private:
    double kp_, ki_, kd_;
    double targetSpeed_;
    double integral_, previousError_;
};

#endif // PID_CONTROLLER_H
ß