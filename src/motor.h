#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <sensors.h>
#ifdef __cplusplus
extern "C" {
#endif

// Function to toggle digital output
extern void motor_toggleDigitalOutput(bool state);

// Function to initialize motor control PINS
extern void motor_init(void);

uint16_t motor_countRunningTime(void);
void motor_stopMotor(void);
void motor_startMotor(void);
bool motor_check_if_watering_needed(void);
float motor_get_watering_time(uint16_t averageReading);


#ifdef __cplusplus
}
#endif

#endif // MOTOR_H






