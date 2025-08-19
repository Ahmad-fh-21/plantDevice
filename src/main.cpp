#include <Arduino.h>
#include <sensors.h>
#include <motor.h>
#include <BluetoothSerial.h>
#include <RTC_handler.h>


// struct to hold sensor data
sensors_struct_t sensors ;


enum {
    idle = 0,
    reading_ongoing = 1,
    reading_finished = 2,
    just_waked_up = 3
};
uint8_t operation_State = 0; // Variable to hold operation state



// Sleep parameters
const uint64_t OPERATION_TIME =   3500;  // 11 seconds in milliseconds
const uint64_t SLEEP_TIME =  1000000 /*60000000 */;      // 60 seconds in milliseconds
uint32_t operationStartTime = 0;
bool shouldEnterSleep = false;
uint32_t currentTime = 0;
uint32_t timedifference = 0;
//bool max_in_second = false; // Flag to indicate if maximum readings in a second have been reached
//bool measurment_ongoing = true; // Flag to indicate if measurement is ongoing


// uint8_t watering = 0;
// bool watering_ongoing = false;

// Task handles
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;
TaskHandle_t Task3Handle = NULL;
TaskHandle_t Task4Handle = NULL;
volatile bool tasksRunning = true;


enum {
    ldle = 0,
    reading = 1,
    error = 2,
    finished = 3
};

uint8_t reading_state = ldle; // Initial state






static void main_init_Tasks(void);
void goToSleep(void);

static void main_handle_motor_watering_logic();

// ****************************************************** 2.5 sec fill 100 mili liter 


// Task functions
// the time needed to measure the soil moisture is 20 ms each time , each second is 10 measurements
void Task40ms(void *parameters) 
{

    while(tasksRunning) 
    {   
        // the system shall read each 40 ms 1 time , the total number should be 10  and stop 
        if ( (sensors.counter_readings < MAX_READINGS && sensors.counter_readings >= 0 )
            && reading_state == reading ) 
        {
            // Read sensor data and store it in the array
            sensors.listofSensorsReadings[sensors.counter_readings] = sensors_readADC();
            sensors.counter_readings++;
        } 
        if (sensors.counter_readings == MAX_READINGS) 
        {

            reading_state = finished; // Set state to finished when maximum readings are reached
        }


        vTaskDelay(pdMS_TO_TICKS(40));
    }
    vTaskDelete(NULL); // Delete this task after completion
}


/*
float motor_handle_watering(void)
{
    uint32_t sum = 0;
    uint8_t new_Start_index = 0;
    
    float wateringTime = 0.0;
    new_Start_index = RTC_getReadingIndex() - 1 ; // Start from the last 9 readings

    for (int i = new_Start_index; i > ( new_Start_index - 24); i--) 
    {
        sum += RTC_get_Reading_History(i);
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(RTC_get_Reading_History(i));

    }

    sum /= 24; // Calculate average of last 3 days readings
    Serial.println("Average of last 3 days readings: " + String(sum));
    
    if( motor_get_watering_time(sum) >= 5) 
    {
        measurment_ongoing = false; // Stop measurements during watering
       // Serial.println("before old" + String(RTC_getBootCount(false)) + "new boot count" + String(RTC_getBootCount(true)));
        wateringTime = motor_get_watering_time(sum);
        Serial.println("Watering time: " + String(wateringTime) + " seconds");
       // Serial.println("after old" + String(RTC_getBootCount(false)) + "new boot count" + String(RTC_getBootCount(true)));
    } 
    else 
    {
       // 
        wateringTime = 0; // No watering needed
    }
    return wateringTime;
}

*/


void Task500ms(void *parameters) 
{
    while(tasksRunning ) 
    {
        if (reading_state == finished )
        {
            sensors.counter_readings = 0; // Reset counter after reading
            calculateAverage(&sensors);    // here calcaulate the average of the readings
            reading_state = ldle; // Reset state to idle after processing readings
           
        }
        else  // if reading is not finished the system shall stop all reading however the state is
        {
            
        }
        if (shouldEnterSleep == true) 
        {
            goToSleep();
        }
   
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL); // Delete this task after completion
}

// void Task3(void *parameters) {
//     while(tasksRunning ) {
//         Serial.println("Task 3 running - 50ms");
//         vTaskDelay(pdMS_TO_TICKS(50));
//     }
// }



void Task1sec(void *parameters) 
{
    while(tasksRunning ) 
    {
        
        if (operation_State != reading_finished)
        {
            if ( sensors.counterAllreadings < TOTAL_READING_ROUTINE)
            {
            if (operation_State == just_waked_up )
            {
                reading_state = reading;
                operation_State = reading_ongoing; // Set state to reading ongoing
               // break; // Exit the loop to start reading
            } 
            else
            {
                Serial.println("Reading complete, processing readings... num of all reading :" + String(sensors.counterAllreadings));
                reading_state = reading; // Set state to read again if the ROUTINE is not finished
                sensors.counterAllreadings++; // Increment counter for all readings
                //sensors.readingComplete = true;  
            }

       
            }
            if (sensors.counterAllreadings  == TOTAL_READING_ROUTINE) // the number of total Routines is achived , turn off reading
            {
                sensors.counter_readings = 0;
                sensors.counterAllreadings = 0;
                operation_State = reading_finished; // Set state to finished
                shouldEnterSleep = true; 
                for (int i = 0; i < RTC_getReadingIndex()  ; i++) 
                {
                    Serial.println("ALL reading History " + String(i ) + ": " + String(RTC_get_Reading_History(i)));
                }

            }
        }





            currentTime = millis(); 
            timedifference = currentTime - operationStartTime; // Calculate time difference since operation start
            // Serial.println("currentTime: " + String(currentTime));
            // Serial.println("Operation start time: " + String(operationStartTime));
            // Serial.println("millis " + String(millis()));
            Serial.println("Timedifference: " + String(timedifference));
            //Serial.println("CounterofAllreading  " + String(sensors.counterAllreadings));


            // Check if operation time has exceeded
            if(timedifference >= OPERATION_TIME  ) 
            {
                Serial.println("boot count: " + String(RTC_getBootCount(false)) + " new boot count: " + String(RTC_getBootCount(true)));
                Serial.println("Operation time exceeded, going to sleep");
                shouldEnterSleep = true; 
            }
            
        
       
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL); // Delete this task after completion
}

void setup() 
{
    Serial.begin(115200);
    delay(1000); // Give time for serial to initialize
    
    // Record start time
    shouldEnterSleep = false; // Initialize sleep flag
    timedifference = 0; // Reset time difference
    currentTime = 0; // Reset current time
    operationStartTime = millis();
    Serial.println("Starting time ..." + String(operationStartTime));
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);

    // Check if this is a wake up from deep sleep
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    if(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) 
    {
        Serial.println("Woken up from deep sleep");
    } 
    else 
    {
        Serial.println("First boot");
        RTC_init(); // Initialize RTC data
    }

    operation_State = just_waked_up; // Set operation state to just woken up
    RTC_incrementBootCount(); // Increment boot count


    pinMode(BUILTIN_LED, OUTPUT);
    //digitalWrite(BUILTIN_LED, HIGH);

    /************************** init Sector ****************************************** */ 
    sensors = sensors_init();            // Initialize sensors
    delay(10);                           // Give time for sensor initialization
    Serial.println("Sensor initialization complete");

    motor_init();                       // Initialize motor control
    Serial.println("Motor initialization complete");

    main_init_Tasks();                   // Initialize tasks
    Serial.println("Tasks initialized");

    /************************** End of init Sector *********************************** */ 


}

void loop() {
 

    
   // vTaskDelay(pdMS_TO_TICKS(100));
}

// Function to initialize tasks
static void main_init_Tasks(void)
{
    // Create tasks
    xTaskCreate(
        Task40ms,          // Task function
        "Task40ms",        // Task name
        2048,              // Stack size
        NULL,              // Parameters
        2,                 // Priority
        &Task1Handle       // Task handle
    );

    xTaskCreate(
        Task500ms,
        "Task500ms",
        2048,
        NULL,
        3,
        &Task2Handle
    );

    // xTaskCreate(
    //     Task3,
    //     "Task3",
    //     2048,
    //     NULL,
    //     1,
    //     &Task3Handle
    // );

    xTaskCreate(
        Task1sec,
        "Task1sec",
        2048,
        NULL,
        2,
        &Task4Handle
    );
}


// Function to handle deep sleep
void goToSleep(void) 
{
    // Turn off all outputs
    Serial.println("Preparing for deep sleep...");
    
    digitalWrite(BUILTIN_LED, LOW); 
    
    // Signal tasks to stop gracefully (if using the safe pattern above)
    tasksRunning = false;
    
    // Wait for tasks to self-terminate
    vTaskDelay(pdMS_TO_TICKS(1000));
    
 
    Task1Handle = NULL;
    Task2Handle = NULL;
    // Task3Handle = NULL;
    Task4Handle = NULL;
    
    // Final cleanup delay
    vTaskDelay(pdMS_TO_TICKS(100));
    
    Serial.println("All tasks deleted");
    Serial.println("Entering deep sleep for 60 seconds");
    Serial.flush();
    
    // Give serial time to complete transmission
    delay(100);
    
    // Enter deep sleep - scheduler will be stopped automatically
    esp_deep_sleep_start();
}


/*
// Function to handle motor watering logic - called in task 1 sec
static void main_handle_motor_watering_logic()
{
    if (motor_check_if_watering_needed() == true) 
        {
            //Serial.println(" 3 days passed, watering is checked");
            if (motor_handle_watering() > 0)
            {
                watering = 2 * motor_handle_watering();
                // Handle watering logic
                watering_ongoing = true; // Set flag to indicate watering is ongoing
            }
            else
            {
                //RTC_setBootCount((RTC_getBootCount(false) - 3 ),true); // delay the next check for 1 day
                watering = 0; // No watering needed
                watering_ongoing = false; // Reset flag when no watering is needed
                //Serial.println("No watering needed at this time");
            }

            Serial.println("Watering started for " + String(watering) + " seconds / 2");
        } 
}
*/

