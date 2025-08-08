#include <sensors.h>


// Pin definitions
const int ADC_PIN = 34;  // GPIO34 for analog input

//const int DIGITAL_IN_PIN = 35;  no need to be used 

// ADC variables
const int ADC_READINGS = 10;




// Function to read and average ADC values
extern float sensors_readADC() {
    // Configure ADC for 12-bit resolution (0-4095)
    analogReadResolution(12);  // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);  // Set ADC attenuation for 3.3V range
    
    // Take multiple readings and average them
    uint32_t sum = 0;  // Using uint32_t to prevent overflow
    for(int i = 0; i < ADC_READINGS; i++) {
        sum += analogRead(ADC_PIN);  // Now reads in 12-bit range (0-4095)
        delay(1);  // Small delay between readings
    }
    int adcValue = sum / ADC_READINGS;
    
    // Convert to voltage (12-bit resolution: 4095 = 3.3V)
    float voltage = (adcValue * 3.3) / 4095.0;
    
    return voltage;
}
