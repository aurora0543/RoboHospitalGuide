# ⚙️ Development Log: Issues Encountered During Debugging
During the development and debugging process, we faced several technical and organizational challenges:

1.	Hardware PWM Issues:
We attempted various methods to access and utilize the hardware PWM for servo control. However, no matter how we configured it, the system failed to export the PWM pin properly. After extensive testing, we concluded that the issue likely lies at the hardware level. As a result, we had to fall back on using software-based PWM. This led to significant imprecision in controlling the servo rotation, particularly affecting the turning mechanism of the robot. Unfortunately, we haven’t found a reliable way to resolve this. Increasing the CPU priority for the PWM process only resulted in system instability and even crashes.

2.	Face Detection Challenges:
The C++-based face recognition models we initially tried did not perform well in our setup. If we had been able to use Python-based models, the accuracy and reliability might have been significantly better. In the end, we resorted to using the built-in face detection model provided by OpenCV, which was sufficient for our basic needs but far from optimal.

3.	Ultrasonic Navigation Limitations:
Our current obstacle avoidance and path planning rely entirely on ultrasonic sensors, which proved to be highly unreliable. One major reason is the lack of hardware resources; unlike back in our home country where we have access to more development platforms and tools, here we had to build everything from scratch, which has been extremely challenging.

4.	Gyroscope Instability:
The gyroscope we used for determining direction and rotation was highly sensitive to noise and produced unstable readings, making angle-based turning control very difficult and inconsistent.

5.	Team Coordination Problems:
At the beginning of the project, we failed to define clear task assignments within the team. As a result, the workload distribution was uneven — some team members ended up doing significantly more than others, which affected overall efficiency and collaboration.
