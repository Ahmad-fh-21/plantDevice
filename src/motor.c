#include <motor.h>

const int DIGITAL_OUT_PIN = 32; // output to control gate of MOSFET




// Function to toggle digital output
extern void motor_toggleDigitalOutput(bool state) {
    digitalWrite(DIGITAL_OUT_PIN, state);
}

// Turn on the motor
void motor_startMotor(void) {
    motor_toggleDigitalOutput(HIGH);  
}

 // Turn off the motor
void motor_stopMotor(void) {
    motor_toggleDigitalOutput(LOW); 
}

uint16_t motor_countRunningTime(void) {
    static unsigned long startTime = 0;
    if (startTime == 0) {
        startTime = millis(); // Start counting time
    }
    return (millis() - startTime) / 1000; // Return time in seconds
}

// Function to initialize motor control PINS
extern void motor_init(void)
{
    pinMode(DIGITAL_OUT_PIN, OUTPUT);
}

// Function to check if watering is needed based on boot count 9 boots difference is equivalent to 3 days
bool motor_check_if_watering_needed(void) 
{

    bool is_watering = false;
    if (RTC_getBootCount(false) - RTC_getBootCount(true) >= 9) 
    {

        RTC_setBootCount(RTC_getBootCount(false),true); // set old count to new count 
        // Serial.println(" old boot count: " + String(RTC_getBootCount(false)) + " new boot count: " + String(RTC_getBootCount(true)));
        // Serial.println("the current index is: " + String(RTC_getReadingIndex()));
        is_watering = true; // Motor is watering
    } 
    else 
    {
        is_watering = false; // Motor is not watering
    }
    return is_watering;
}

float motor_get_watering_time(uint16_t averageReading)
{   
    float wateringTime = 0.0;

    // Determine soil state based on the average reading
    if (averageReading >= VERY_DRY_SOIL_THRESHOLD) 
    {
        wateringTime = 7.5; // 7.5 seconds for very dry soil 300ml
    } 
    else if (averageReading >=  DRY_SOIL_THRESHOLD  && averageReading < VERY_DRY_SOIL_THRESHOLD ) 
    {
        wateringTime = 5.0; // 5 seconds for dry soil 200ml
    } 
    else if (averageReading >= WET_SOIL_THRESHOLD  && averageReading < DRY_SOIL_THRESHOLD ) 
    {
       wateringTime = 0; // No watering needed for moist soil
    }
    else if (averageReading >= VERY_WET_SOIL_THRESHOLD && averageReading < WET_SOIL_THRESHOLD )
    {
        wateringTime = 0; // No watering needed for wet soil
    }
    else if (averageReading < VERY_WET_SOIL_THRESHOLD && averageReading >= 0) 
    {
        wateringTime = 0; // No watering needed for very wet soil
    }
 
    return wateringTime;
}
