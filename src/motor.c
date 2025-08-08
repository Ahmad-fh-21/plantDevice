#include <motor.h>

const int DIGITAL_OUT_PIN = 32; // output to control gate of MOSFET




// Function to toggle digital output
extern void motor_toggleDigitalOutput(bool state) {
    digitalWrite(DIGITAL_OUT_PIN, state);
}

// Function to initialize motor control PINS
extern void motor_init(void)
{
    pinMode(DIGITAL_OUT_PIN, OUTPUT);
}

