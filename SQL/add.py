import pymysql
import random
from datetime import datetime, timedelta
from faker import Faker

# 初始化 Faker
fake = Faker()

# 数据库连接配置（根据你实际情况修改）
conn = pymysql.connect(
    host="localhost",
    user="remote_user",
    password="",
    database="HospitalGuide",
    charset="utf8mb4"
)


try:
    with conn.cursor() as cursor:
        # 获取所有患者 ID
        cursor.execute("SELECT PatientID FROM Patients")
        patient_ids = [row[0] for row in cursor.fetchall()]

        # 获取所有科室 ID（1 到 11）
        cursor.execute("SELECT DepartmentID FROM Departments")
        department_ids = [row[0] for row in cursor.fetchall()]

        for pid in patient_ids:
            dept_id = random.choice(department_ids)

            # 生成未来 7 天内的随机时间（上午 8 点到下午 5 点之间）
            random_day = random.randint(0, 6)
            random_hour = random.randint(8, 17)
            random_minute = random.choice([0, 15, 30, 45])
            appointment_time = datetime.now() + timedelta(days=random_day)
            appointment_time = appointment_time.replace(hour=random_hour, minute=random_minute, second=0, microsecond=0)

            # 插入挂号信息
            sql = """
            INSERT INTO Registrations (PatientID, DepartmentID, AppointmentTime, AdditionalNotes)
            VALUES (%s, %s, %s, %s)
            """
            cursor.execute(sql, (pid, dept_id, appointment_time, None))

    conn.commit()
    print("✅ 成功为所有病人添加挂号记录。")

finally:
    conn.close()