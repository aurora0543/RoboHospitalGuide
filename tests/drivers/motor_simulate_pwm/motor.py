import RPi.GPIO as GPIO
import time
import threading

# **定义 GPIO 引脚**
E1A = 20
E1B = 21
E2A = 26
E2B = 27
AIN1 = 17
AIN2 = 18
BIN1 = 22
BIN2 = 23

# **设置 GPIO**
GPIO.setmode(GPIO.BCM)
GPIO.setup(AIN1, GPIO.OUT)
GPIO.setup(AIN2, GPIO.OUT)
GPIO.setup(BIN1, GPIO.OUT)
GPIO.setup(BIN2, GPIO.OUT)
GPIO.setup(E1A, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(E1B, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(E2A, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(E2B, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# **PWM 设置**
pwm_A = GPIO.PWM(AIN1, 100)  # 100Hz 频率
pwm_B = GPIO.PWM(BIN1, 100)
pwm_A.start(0)  # 初始占空比 0%
pwm_B.start(0)

# **记录当前 PWM 占空比**
current_duty_A = 50  # 默认 50%
current_duty_B = 50
pwm_A.ChangeDutyCycle(current_duty_A)
pwm_B.ChangeDutyCycle(current_duty_B)

# **编码器变量**
encoder_count_A = 0
encoder_count_B = 0
target_speed = 50  # 目标速度（占空比 50%）
Kp = 0.5  # 比例系数

# **编码器中断回调**
def encoder_callback_A(channel):
    global encoder_count_A
    encoder_count_A += 1

def encoder_callback_B(channel):
    global encoder_count_B
    encoder_count_B += 1

# **注册中断**
GPIO.add_event_detect(E1A, GPIO.RISING, callback=encoder_callback_A)
GPIO.add_event_detect(E2A, GPIO.RISING, callback=encoder_callback_B)

# **计算转速**
def calculate_speed():
    global encoder_count_A, encoder_count_B, current_duty_A, current_duty_B
    while True:
        count_A = encoder_count_A
        count_B = encoder_count_B
        encoder_count_A = 0
        encoder_count_B = 0

        speed_A = count_A / 20  # 假设编码器 20 脉冲 = 1 转
        speed_B = count_B / 20
        print(f"速度 A: {speed_A} 转/秒, 速度 B: {speed_B} 转/秒")

        # **PID 控制调整 PWM**
        error_A = target_speed - speed_A
        error_B = target_speed - speed_B

        current_duty_A = max(0, min(100, current_duty_A + Kp * error_A))
        current_duty_B = max(0, min(100, current_duty_B + Kp * error_B))

        pwm_A.ChangeDutyCycle(current_duty_A)
        pwm_B.ChangeDutyCycle(current_duty_B)

        time.sleep(1)

# **启动速度计算线程**
threading.Thread(target=calculate_speed, daemon=True).start()

try:
    while True:
        time.sleep(1)  # 保持程序运行
except KeyboardInterrupt:
    print("停止电机")
    pwm_A.stop()
    pwm_B.stop()
    GPIO.cleanup()