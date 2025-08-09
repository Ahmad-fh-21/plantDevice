#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function to toggle digital output
extern void motor_toggleDigitalOutput(bool state);

// Function to initialize motor control PINS
extern void motor_init(void);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_H






