#include <sensors.h>


// // main Variables
// float listofSensorsReadings[50] = {}; // Array to store sensor readings
// int averageIndex_readings[5] = {}; // Index for the next sensor reading
// int counter_readings = 0; // Counter for the number of readings
// bool readingComplete = false; // Flag to indicate if readings are complete
// bool fullReadingProcess_Complete = false; // Counter for seconds 
// uint8_t counterAllreadings = 0; // Counter for all readings



// uint8_t listofsoilStates[3] = {}; // Array to store soil states for each sensor

// Initialize sensor power control
sensors_struct_t sensors_init() {
    sensors_struct_t sensors;
    pinMode(SENSOR_POWER, OUTPUT);
    digitalWrite(SENSOR_POWER, LOW);  // Start with sensor off

        // Configure ADC for 12-bit resolution (0-4095)
    analogReadResolution(12);  // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);  // Set ADC attenuation for 3.3V range
    sensors.counter_readings = 0;  // Initialize reading counter
    sensors.readingComplete = false;  // Initialize reading complete flag
    sensors.fullReadingProcess_Complete = false;  // Initialize full reading process flag
    sensors.counterAllreadings = 0;  // Initialize all readings counter

    for(int i = 0; i < MAX_READINGS; i++) 
    {
        sensors.listofSensorsReadings[i] = 0;  // Initialize sensor readings array
    }
    for(int i = 0; i < AVERAGE_READINGS; i++) 
    {
        sensors.averageIndex_readings[i] = 0;  // Initialize average index readings
    }
    for(int i = 0; i < 3; i++) 
    {
        sensors.listofsoilStates[i] = 0;  // Initialize soil states array
    }
    
    return sensors;
}

// Function to control sensor power
void sensors_setPower(bool state) {
    digitalWrite(SENSOR_POWER, state);
    if(state) {
        delay(10);  // Give sensor time to stabilize
    }
}


// Function to read and average ADC values
extern float sensors_readADC(void) {

    // Power up sensor before reading
    sensors_setPower(HIGH);

    // Take multiple readings and average them
    uint32_t sum = 0;  // Using uint32_t to prevent overflow
    for(int i = 0; i < ADC_READINGS; i++) {
        sum += analogRead(ADC_PIN);  // Now reads in 12-bit range (0-4095)
        delay(1);  // Small delay between readings
    }
    int adcValue = sum / ADC_READINGS;
    
    // Convert to voltage (12-bit resolution: 4095 = 3.3V)
   // float voltage = (adcValue * 3.3) / 4095.0;
    sensors_setPower(LOW);  // Turn off sensor after reading
    return adcValue;
}

// // Function to print soil state based on sensor reading
const char* sensors_getSoilStateString(uint8_t soilState) {
    switch (soilState) {
        case VERY_DRY_SOIL:
            return "Very Dry Soil";
        case DRY_SOIL:
            return "Dry Soil";
        case MOIST_SOIL:
            return "Moist Soil";   
        case WET_SOIL:
            return "Wet Soil";
        case VERY_WET_SOIL:
            return "Very Wet Soil";
        default:
            return "Unknown Soil State";
    }
}

// Function to get the soil state based on sensor readings
uint16_t sensors_getSoilState(sensors_struct_t *sensors) {
    // Get the average of the sensor readings
    uint16_t averageReading = 0;
    uint8_t soilstate = 0; // Variable to store soil state

    for (int i = 0; i < 5; i++) {
        averageReading += sensors->listofSensorsReadings[i];
    }
    averageReading /= 5;

    //Serial.print("Average reading: ");
    //Serial.println(averageReading);

    // Determine soil state based on the average reading
    if (averageReading >= VeryDrySoil) 
    {
        soilstate = VERY_DRY_SOIL;
    } 
    else if (averageReading >= DrySoil && averageReading < VeryDrySoil) 
    {
        soilstate = DRY_SOIL;
    } 
    else if (averageReading >= WetSoil && averageReading < DrySoil) 
    {
        soilstate = MOIST_SOIL;
    }
    else if (averageReading >= VeryWetSoil && averageReading < WetSoil)
    {
        soilstate = WET_SOIL;
    }
    else if (averageReading < VeryWetSoil && averageReading >= 0) 
    {
        soilstate = VERY_WET_SOIL;
    }

    return soilstate;
}
