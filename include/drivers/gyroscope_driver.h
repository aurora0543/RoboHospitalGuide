/**
 * @file gyroscope_driver.h
 * @brief Header file for gyroscope driver
 * 
 * This file declares functions for initializing and reading data 
 * from the gyroscope sensor.
 * 
 * @author [Your Name]
 * @date 2025-02-16
 * @license MIT
 */

 #ifndef GYROSCOPE_DRIVER_H
 #define GYROSCOPE_DRIVER_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 #include <stdint.h>
 
 // Gyroscope data structure
 typedef struct {
     float x;
     float y;
     float z;
 } GyroscopeData;
 
 // Initialize the gyroscope
 void gyroscope_init(void);
 
 // Read gyroscope data
 void gyroscope_read(GyroscopeData* data);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* GYROSCOPE_DRIVER_H */