import smbus
import time
import math
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

# MPU6050 寄存器配置
MPU6050_ADDR = 0x68
PWR_MGMT_1 = 0x6B
CONFIG = 0x1A
GYRO_CONFIG = 0x1B
ACCEL_CONFIG = 0x1C
ACCEL_XOUT_H = 0x3B
GYRO_XOUT_H = 0x43

# 初始化 I2C
bus = smbus.SMBus(1)
bus.write_byte_data(MPU6050_ADDR, PWR_MGMT_1, 0x01)  # 唤醒设备
bus.write_byte_data(MPU6050_ADDR, CONFIG, 0x03)      # DLPF 44Hz
bus.write_byte_data(MPU6050_ADDR, ACCEL_CONFIG, 0x00)  # ±2g
bus.write_byte_data(MPU6050_ADDR, GYRO_CONFIG, 0x18)   # ±2000dps

def read_word(reg):
    high = bus.read_byte_data(MPU6050_ADDR, reg)
    low = bus.read_byte_data(MPU6050_ADDR, reg + 1)
    value = (high << 8) + low
    return value if value < 0x8000 else value - 0x10000

# 初始化3D图形
plt.ion()
fig = plt.figure(figsize=(8,6))
ax = fig.add_subplot(111, projection='3d')
ax.set_xlim(-2, 2)
ax.set_ylim(-2, 2)
ax.set_zlim(-2, 2)

# 立方体定义
faces = [
    {"verts": [0,1,2,3], "color": "red"},    
    {"verts": [4,5,6,7], "color": "darkred"},
    {"verts": [0,3,7,4], "color": "lime"},   
    {"verts": [1,2,6,5], "color": "green"},
    {"verts": [0,1,5,4], "color": "blue"},   
    {"verts": [2,3,7,6], "color": "navy"}
]

vertices = np.array([
    [-1,-1,-1], [1,-1,-1], [1,1,-1], [-1,1,-1],
    [-1,-1,1], [1,-1,1], [1,1,1], [-1,1,1]
], dtype=np.float32)

# 传感器融合参数
alpha = 0.98  # 互补滤波系数
dt = 0.02      # 采样间隔

# 姿态参数
roll, pitch, yaw = 0.0, 0.0, 0.0
gyro_offset = np.zeros(3)  # 陀螺仪校准偏移

def calibrate_gyro():
    print("校准陀螺仪...保持静止")
    samples = 50
    offsets = np.zeros(3)
    for _ in range(samples):
        offsets += np.array([read_word(GYRO_XOUT_H), 
                           read_word(GYRO_XOUT_H+2),
                           read_word(GYRO_XOUT_H+4)])
        time.sleep(0.01)
    return offsets / samples

def update_attitude(accel, gyro):
    global roll, pitch, yaw
    
    # 加速度计姿态计算
    acc_roll = math.atan2(-accel[0], math.sqrt(accel[1]**2 + accel[2]**2))
    acc_pitch = math.atan2(accel[1], math.sqrt(accel[0]**2 + accel[2]**2))
    
    # 陀螺仪积分（转换为弧度）
    gyro_rad = np.radians(gyro / 131.0)  # ±2000dps对应131 LSB/(d/s)
    roll += gyro_rad[0] * dt
    pitch += gyro_rad[1] * dt
    yaw += gyro_rad[2] * dt
    
    # 互补滤波融合
    roll = alpha * roll + (1 - alpha) * acc_roll
    pitch = alpha * pitch + (1 - alpha) * acc_pitch

def rotation_matrix():
    # 绕Z-Y-X顺序旋转（Yaw-Pitch-Roll）
    cy = math.cos(yaw)
    sy = math.sin(yaw)
    cp = math.cos(pitch)
    sp = math.sin(pitch)
    cr = math.cos(roll)
    sr = math.sin(roll)
    
    return np.array([
        [cy*cp, cy*sp*sr - sy*cr, cy*sp*cr + sy*sr],
        [sy*cp, sy*sp*sr + cy*cr, sy*sp*cr - cy*sr],
        [-sp, cp*sr, cp*cr]
    ])

def update_cube():
    rotated = vertices @ rotation_matrix().T
    
    ax.cla()
    ax.set_xlim(-2, 2)
    ax.set_ylim(-2, 2)
    ax.set_zlim(-2, 2)
    
    for face in faces:
        verts = rotated[face["verts"]]
        poly = Poly3DCollection([verts], alpha=0.8)
        poly.set_facecolor(face["color"])
        ax.add_collection3d(poly)

try:
    # 校准陀螺仪
    gyro_offset = calibrate_gyro()
    print(f"校准完成，偏移量: {gyro_offset}")
    
    while True:
        start_time = time.time()
        
        # 读取传感器数据
        accel = np.array([
            read_word(ACCEL_XOUT_H)/16384.0,
            read_word(ACCEL_XOUT_H+2)/16384.0,
            read_word(ACCEL_XOUT_H+4)/16384.0
        ])
        
        gyro = np.array([
            read_word(GYRO_XOUT_H) - gyro_offset[0],
            read_word(GYRO_XOUT_H+2) - gyro_offset[1],
            read_word(GYRO_XOUT_H+4) - gyro_offset[2]
        ])
        
        # 更新姿态
        update_attitude(accel, gyro)
        update_cube()
        
        plt.draw()
        plt.pause(max(0.001, dt - (time.time() - start_time)))
        
except KeyboardInterrupt:
    plt.close()
    print("程序终止")