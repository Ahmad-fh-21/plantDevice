#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <RTC_handler.h>

#ifdef __cplusplus
extern "C" {
#endif

// Soil moisture thresholds
const uint16_t VeryDrySoil = 3900; // Very dry soil threshold
const uint16_t DrySoil = 3000; // Dry soil threshold
const uint16_t WetSoil = 2000; // Wet soil threshold
const uint16_t VeryWetSoil = 1000; // Moist soil threshold

// Pin definitions
const int ADC_PIN = 34;  // GPIO34 for analog input
const int SENSOR_POWER = 25;  // GPIO25 for sensor power control & Supply

// ADC variables
const int ADC_READINGS = 10;

#define MAX_READINGS  50 // Maximum number of readings to store
#define AVERAGE_READINGS  5 // Number of readings to average
#define SOIL_STATES 3 // Number of soil states to track



typedef struct {
    // main Variables
        float listofSensorsReadings[MAX_READINGS]; // Array to store sensor readings
        uint16_t averageIndex_readings[AVERAGE_READINGS]; // Index for the next sensor reading
        uint8_t counter_readings ; // Counter for the number of readings
        bool readingComplete ; // Flag to indicate if readings are complete
        bool fullReadingProcess_Complete ; // Counter for seconds 
        uint8_t counterAllreadings ; // Counter for all readings
        uint8_t listofsoilStates[SOIL_STATES] ; // Array to store soil states for each sensor
}sensors_struct_t;

// Soil state enumeration
enum SoilState {
    VERY_DRY_SOIL = 0,
    DRY_SOIL = 1,
    MOIST_SOIL = 2,
    WET_SOIL = 3,   
    VERY_WET_SOIL = 4

};


// Function declarations
sensors_struct_t sensors_init();
void sensors_setPower(bool state);
float sensors_readADC(void);
const char* sensors_getSoilStateString(uint8_t soilState);
uint8_t sensors_getSoilState(sensors_struct_t *sensors);

#ifdef __cplusplus
}
#endif

#endif // SENSORS_H