#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <RTC_handler.h>

#ifdef __cplusplus
extern "C" {
#endif


// sensors Constants
#define VERY_DRY_SOIL_THRESHOLD  3900
#define DRY_SOIL_THRESHOLD       3000
#define WET_SOIL_THRESHOLD       2000
#define VERY_WET_SOIL_THRESHOLD  1000

#define ADC_PIN         34   // GPIO34 for analog input
#define SENSOR_POWER    25   // GPIO25 for sensor power control & Supply
#define ADC_READINGS    10   // Number of ADC readings to average

#define MAX_READINGS       10 // Maximum number of readings to store
#define AVERAGE_READINGS   3  // Number of readings to average
#define SOIL_STATES        3  // Number of soil states to track

typedef struct 
{
    // main Variables
    float    listofSensorsReadings[MAX_READINGS]; // Array to store sensor readings
    uint16_t averageIndex_readings[AVERAGE_READINGS]; // Index for the next sensor reading
    uint8_t  counter_readings ; // Counter for the number of readings
    bool     readingComplete ; // Flag to indicate if readings are complete
    bool     fullReadingProcess_Complete ; // Counter for seconds 
    uint8_t  counterAllreadings ; // Counter for all readings
    uint8_t  listofsoilStates[SOIL_STATES] ; // Array to store soil states for each sensor

    uint8_t  averageIndex ; // Index for the next average reading
    bool     readingInProgress_in_1_sec; // Flag to indicate if a reading is in progress

}sensors_struct_t;

// Soil state enumeration
enum SoilState 
{
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
void calculateAverage(sensors_struct_t *sensors);


#ifdef __cplusplus
}
#endif

#endif // SENSORS_H