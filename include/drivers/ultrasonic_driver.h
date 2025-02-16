/**
 * @file ultrasonic_driver.h
 * @brief Header file for ultrasonic sensor driver
 * 
 * This file provides functions to initialize and read distance from 
 * an ultrasonic sensor.
 * 
 * @author [Your Name]
 * @date 2025-02-16
 * @license MIT
 */

 #ifndef ULTRASONIC_DRIVER_H
 #define ULTRASONIC_DRIVER_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 #include <stdint.h>
 
 // Initialize ultrasonic sensor
 void ultrasonic_init(void);
 
 // Get distance reading in centimeters
 float ultrasonic_get_distance(void);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* ULTRASONIC_DRIVER_H */