#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
void sensors_init(void);
void sensors_setPower(bool state);
float sensors_readADC(void);

#ifdef __cplusplus
}
#endif

#endif // SENSORS_H