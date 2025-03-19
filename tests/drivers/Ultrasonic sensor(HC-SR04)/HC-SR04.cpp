#include <wiringPi.h>
#include <iostream>
#include <unistd.h>   // For usleep()
#include <sys/time.h> // For gettimeofday()

#define TRIG  24  // Trigger pin (GPIO 23, Pin 16)
#define ECHO  25  // Echo pin (GPIO 24, Pin 18)

// Get current time in microseconds
long getMicrotime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

// Measure distance using HC-SR04
float getDistance() {
    // Send ultrasonic pulse
    digitalWrite(TRIG, LOW);
    usleep(2);
    digitalWrite(TRIG, HIGH);
    usleep(10);
    digitalWrite(TRIG, LOW);

    // Wait for Echo to go HIGH (start time)
    while (digitalRead(ECHO) == LOW);
    long startTime = getMicrotime();

    // Wait for Echo to go LOW (end time)
    while (digitalRead(ECHO) == HIGH);
    long travelTime = getMicrotime() - startTime;

    // Calculate distance (in cm), speed of sound = 0.0343 cm/µs
    float distance = travelTime * 0.0343 / 2;
    return distance;
}

int main() {
    // Initialize WiringPi
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "WiringPi initialization failed!" << std::endl;
        return 1;
    }

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    std::cout << "HC-SR04 Ultrasonic Sensor Test Started..." << std::endl;

    while (true) {
        float distance = getDistance();
        std::cout << "Measured Distance: " << distance << " cm" << std::endl;

        if (distance < 20.0) {
            std::cout << "⚠ Warning: Obstacle too close! Stop or turn!" << std::endl;
        }

        sleep(1); // Measure every 1 second
    }

    return 0;
}
