/**
 * @file motor_driver.h
 * @brief Header file for motor driver
 * 
 * This file provides functions to control the motors, including speed and direction.
 * 
 * @author [Your Name]
 * @date 2025-02-16
 * @license MIT
 */

 #ifndef MOTOR_DRIVER_H
 #define MOTOR_DRIVER_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 #include <stdint.h>
 
 // Motor directions
 typedef enum {
     MOTOR_FORWARD,
     MOTOR_BACKWARD,
     MOTOR_STOP
 } MotorDirection;
 
 // Initialize the motor driver
 void motor_init(void);
 
 // Set motor speed (0-100)
 void motor_set_speed(uint8_t speed);
 
 // Set motor direction
 void motor_set_direction(MotorDirection direction);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* MOTOR_DRIVER_H */