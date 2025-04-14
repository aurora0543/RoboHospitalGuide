import time
import os

PWM_CHIP = "/sys/class/pwm/pwmchip0"
PWM0 = os.path.join(PWM_CHIP, "pwm0")

def write(path, value):
    with open(path, 'w') as f:
        f.write(str(value))

def setup_pwm():
    if not os.path.exists(PWM0):
        write(os.path.join(PWM_CHIP, "export"), 0)
        time.sleep(0.1)

    write(os.path.join(PWM0, "period"), 20000000)  # 20ms -> 50Hz
    write(os.path.join(PWM0, "enable"), 1)

def set_angle_us(pulse_width_us):
    # pulse_width_us in microseconds
    duty_ns = pulse_width_us * 1000
    write(os.path.join(PWM0, "duty_cycle"), duty_ns)

def test_servo():
    print("⚙️ 初始化 PWM 输出...")
    setup_pwm()

    try:
        while True:
            print("🔄 中位 (1.5ms)")
            set_angle_us(1500)
            time.sleep(1)

            print("↘️ 右转 (2.0ms)")
            set_angle_us(2000)
            time.sleep(1)

            print("↙️ 左转 (1.0ms)")
            set_angle_us(1000)
            time.sleep(1)

    except KeyboardInterrupt:
        print("🛑 退出，舵机回正")
        set_angle_us(1500)
        write(os.path.join(PWM0, "enable"), 0)

if __name__ == "__main__":
    test_servo()