#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <gpiod.hpp>
#include <functional>
#include <atomic>
#include <thread>
#include <string>

/**
 * @brief HC-SR04 Ultrasonic Sensor wrapper that runs distance measurement in the background
 *        and reports results via a callback function.
 */
class UltrasonicSensor {
public:
    using Callback = std::function<void(float)>;

    /**
     * @param chip_name GPIO chip name (e.g., "gpiochip0")
     * @param trig_pin  BCM pin number for the trigger pin
     * @param echo_pin  BCM pin number for the echo pin
     * @param callback  Called after each measurement; argument is the measured distance in cm
     */
    UltrasonicSensor(const std::string& chip_name,
                     int trig_pin,
                     int echo_pin,
                     Callback callback);

    /// Stops measurement and releases resources
    ~UltrasonicSensor();

    // Disable copy operations
    UltrasonicSensor(const UltrasonicSensor&) = delete;
    UltrasonicSensor& operator=(const UltrasonicSensor&) = delete;

private:
    void measureDistance();  ///< Background loop that performs distance measurement

    std::unique_ptr<gpiod::chip> chip;
    gpiod::line              trig_line;
    gpiod::line              echo_line;

    std::atomic<bool> running;
    std::thread       sensor_thread;
    Callback          callback;

    // Hardware pin definitions for multiple ultrasonic sensors
    #define TRIG_FRONT 24
    #define ECHO_FRONT 25
    #define TRIG_LEFT  19
    #define ECHO_LEFT  13
    #define TRIG_RIGHT  6
    #define ECHO_RIGHT  5
    #define TRIG_REAR  12
    #define ECHO_REAR   4
};

#endif // ULTRASONIC_SENSOR_H