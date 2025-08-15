#include <sensors.h>



// Initialize sensor power control
sensors_struct_t sensors_init() 
{
    sensors_struct_t sensors;
    pinMode(SENSOR_POWER, OUTPUT);
    digitalWrite(SENSOR_POWER, LOW);              // Start with sensor off

    // Configure ADC for 12-bit resolution (0-4095)
    analogReadResolution(12);                     // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);               // Set ADC attenuation for 3.3V range
    sensors.counter_readings = 0;                 // Initialize reading counter
    sensors.readingComplete = false;              // Initialize reading complete flag
    sensors.fullReadingProcess_Complete = false;  // Initialize full reading process flag
    sensors.counterAllreadings = 0;               // Initialize all readings counter

    for(int i = 0; i < MAX_READINGS; i++) 
    {
        sensors.listofSensorsReadings[i] = 0;     // Initialize sensor readings array
    }
    for(int i = 0; i < AVERAGE_READINGS; i++) 
    {
        sensors.averageIndex_readings[i] = 0;     // Initialize average index readings
    }
    for(int i = 0; i < 3; i++) 
    {
        sensors.listofsoilStates[i] = 0;          // Initialize soil states array
    }
    
    return sensors;
}

// Function to control sensor power
void sensors_setPower(bool state) 
{
    digitalWrite(SENSOR_POWER, state);

    if(state) 
    {
        delay(10);  // Give sensor time to stabilize
    }
}


// Function to read and average ADC values
extern float sensors_readADC(void) 
{
    // Power up sensor before reading
    sensors_setPower(HIGH);

    // Take multiple readings and average them
    uint32_t sum = 0;                          // Using uint32_t to prevent overflow
    
    for(int i = 0; i < ADC_READINGS; i++) 
    {
        sum += analogRead(ADC_PIN);            // Now reads in 12-bit range (0-4095)
        delay(1);                              // Small delay between readings
    }

    int adcValue = sum / ADC_READINGS;
    
    sensors_setPower(LOW);                     // Turn off sensor after reading
    
    return adcValue;
}

// // Function to print soil state based on sensor reading
const char* sensors_getSoilStateString(uint8_t soilState) 
{
    switch (soilState) 
    {
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
uint8_t sensors_getSoilState(sensors_struct_t *sensors) {
    // Get the average of the sensor readings
    uint16_t averageReading = 0;
    uint8_t soilstate = 0; // Variable to store soil state

    for (int i = 0; i < AVERAGE_READINGS ; i++) 
    {
        averageReading += sensors->listofSensorsReadings[i];
    }

    averageReading /= AVERAGE_READINGS;

    // Save readings to RTC
    RTC_saveReading(averageReading);
    
    // Determine soil state based on the average reading
    if (averageReading >= VERY_DRY_SOIL_THRESHOLD ) 
    {
        soilstate = VERY_DRY_SOIL;
    } 
    else if (averageReading >= DRY_SOIL_THRESHOLD   && averageReading < VERY_DRY_SOIL_THRESHOLD ) 
    {
        soilstate = DRY_SOIL;
    } 
    else if (averageReading >=   WET_SOIL_THRESHOLD  && averageReading < DRY_SOIL_THRESHOLD ) 
    {
        soilstate = MOIST_SOIL;
    }
    else if (averageReading >= VERY_WET_SOIL_THRESHOLD && averageReading < WET_SOIL_THRESHOLD )
    {
        soilstate = WET_SOIL;
    }
    else if (averageReading < VERY_WET_SOIL_THRESHOLD && averageReading >= 0) 
    {
        soilstate = VERY_WET_SOIL;
    }

    return soilstate;
}

// Function to calculate the average of sensor readings
void calculateAverage(sensors_struct_t *sensors) 
{
    if (sensors->readingComplete == true && sensors->counter_readings > 0) 
    {
        float sum = 0;
       
        for (int i = 0; i < sensors->counter_readings; i++) 
        {
            // Serial.println("Reading " + String(i) + ": " + String(sensors->listofSensorsReadings[i]));
            sum += sensors->listofSensorsReadings[i];
            if ((i+1) % 10 == 0 && i != 0) 
            {
                sensors->averageIndex_readings[i / 10] = sum /  10; // Store average every 10 readings
                // Serial.print("sum: ");
                // Serial.println(sum);
                sum = 0; // Reset sum for the next average calculation 

            }
        }
    } 
    else 
    {
        //Serial.println("No readings to average.");
    }
    
}