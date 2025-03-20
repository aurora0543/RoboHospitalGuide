#ifndef PWM_HPP
#define PWM_HPP

#include <string>

class PWMControl {
public:
    PWMControl(int pwm_channel, int frequency);
    ~PWMControl();

    void setDutyCycle(int dutyCycle);
    void enable();
    void disable();

private:
    int pwmChannel;
    std::string pwmPath;
    void writeFile(const std::string& filename, const std::string& value);
};

#endif // PWM_HPP
