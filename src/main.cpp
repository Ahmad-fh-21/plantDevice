#include <Arduino.h>
#include <sensors.h>
#include <motor.h>
#include <BluetoothSerial.h>
#include <RTC_handler.h>

// // main Variables
// float listofSensorsReadings[50] = {}; // Array to store sensor readings
// int averageIndex_readings[5] = {}; // Index for the next sensor reading
// int counter_readings = 0; // Counter for the number of readings
// bool readingComplete = false; // Flag to indicate if readings are complete
// bool fullReadingProcess_Complete = false; // Counter for seconds 
// uint8_t counterAllreadings = 0; // Counter for all readings



// uint8_t listofsoilStates[3] = {}; // Array to store soil states for each sensor



// struct to hold sensor data
sensors_struct_t sensors ;

// Sleep parameters
const uint64_t OPERATION_TIME =   17500;  // 16.5 seconds in milliseconds
const uint64_t SLEEP_TIME =  1000000 /*60000000 */;      // 60 seconds in milliseconds
uint32_t operationStartTime = 0;
bool shouldEnterSleep = false;
uint32_t currentTime = 0;
uint32_t timedifference = 0;
bool max_in_second = false; // Flag to indicate if maximum readings in a second have been reached
bool measurment_ongoing = true; // Flag to indicate if measurement is ongoing


uint8_t watering = 0;
bool watering_ongoing = false;

// Task handles
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;
TaskHandle_t Task3Handle = NULL;
TaskHandle_t Task4Handle = NULL;
volatile bool tasksRunning = true;

/*
// Bluetooth parameters
// Add this check for Bluetooth support
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` enable it
#endif

// Create BluetoothSerial instance
BluetoothSerial SerialBT;

// Add these global variables
const char* DEVICE_NAME = "ESP32_Plant_Monitor";
String btMessage = "";

*/




// ****************************************************** 2.5 sec fill 100 mili liter 
void goToSleep() {
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
    delay(200);
    
    // Enter deep sleep - scheduler will be stopped automatically
    esp_deep_sleep_start();
}

/*
// Add this function to handle Bluetooth messages
void handleBluetoothMessages() {
    if (SerialBT.available()) {
        char inChar = (char)SerialBT.read();
        if (inChar == '\n') {
            // Process complete message
            if (btMessage == "STATUS") {
                // Send sensor status
                SerialBT.printf("Soil States:\n");
                for (int i = 0; i < 3; i++) {
                    SerialBT.printf("Sensor %d: %s\n", 
                        i + 1, 
                        sensors_getSoilStateString(sensors.listofsoilStates[i]));
                }
            } else if (btMessage == "WAKE") {
                SerialBT.println("Waking up device...");
                // Add wake-up logic here
            } else if (btMessage == "SLEEP") {
                SerialBT.println("Going to sleep...");
                shouldEnterSleep = true;
            }
            btMessage = ""; // Clear the message buffer
        } else {
            btMessage += inChar; // Add character to message
        }
    }
}

*/


void calculateAverage(sensors_struct_t *sensors) {
    if (sensors->readingComplete == true && sensors->counter_readings > 0) {
        float sum = 0;
       
        for (int i = 0; i < sensors->counter_readings; i++) {
            // Serial.println("Reading " + String(i) + ": " + String(sensors->listofSensorsReadings[i]));
            sum += sensors->listofSensorsReadings[i];
            if ((i+1) % 10 == 0 && i != 0) {
              sensors->averageIndex_readings[i / 10] = sum /  10; // Store average every 10 readings
            // Serial.print("sum: ");
            // Serial.println(sum);
              sum = 0; // Reset sum for the next average calculation 

            }
        }
        
        for (int i = 0; i < 5; i++) {
            Serial.print("Average reading for sensor ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(sensors->averageIndex_readings[i]);
        }
        
    } else {
        Serial.println("No readings to average.");
    }
    
}




// Task functions
// the time needed to measure the soil moisture is 20 ms each time , each second is 10 measurements
void Task40ms(void *parameters) {
    while(tasksRunning ) {
        //Serial.println("Task 1 running - 100ms");

         //int starttime = millis();
        if ( max_in_second == false)
        {
            if ( sensors.counter_readings < 50 && sensors.counter_readings >= 0 && sensors.fullReadingProcess_Complete == false ) 
            {
            // Read sensor data and store it in the array
            sensors.listofSensorsReadings[sensors.counter_readings] = sensors_readADC();
            sensors.counter_readings++;

            if (sensors.counter_readings % 10 == 0)
            {
                max_in_second = true; // Set flag to indicate maximum readings in a second have been reached
            }

            sensors.readingComplete = false; // Reset flag after reading
            //Serial.println("sensors.counter_readings:  " + String(sensors.counter_readings));
        } else {
            // Reset counter if it exceeds the array size
            sensors.counter_readings = 0;
        }
        }


        // Serial.println("time to measure  " + String(millis() - starttime) + " ms");

        vTaskDelay(pdMS_TO_TICKS(40));
    }
    vTaskDelete(NULL); // Delete this task after completion
}



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
        Serial.println("before old" + String(RTC_getBootCount(false)) + "new boot count" + String(RTC_getBootCount(true)));
        wateringTime = motor_get_watering_time(sum);
        Serial.println("Watering time: " + String(wateringTime) + " seconds");
        Serial.println("after old" + String(RTC_getBootCount(false)) + "new boot count" + String(RTC_getBootCount(true)));
    } 
    else 
    {
       // 
        wateringTime = 0; // No watering needed
    }
    return wateringTime;
}







void Task500ms(void *parameters) {
    while(tasksRunning ) {
        
        if (watering_ongoing == true)
        {
            digitalWrite(BUILTIN_LED, HIGH);
            
            if (watering <= 0)  
            {
                digitalWrite(BUILTIN_LED, LOW);
                watering_ongoing = false; // Reset flag when watering is complete
                Serial.println("Watering complete");
                
            }
            else
            {
                watering--;
            }
        }

        else 
        {
           
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

void main_reading_logic()
{
    if (sensors.counter_readings == 50  )
    {
        sensors.readingComplete = true; // Set flag when readings are complete
        calculateAverage(&sensors); // Calculate the average of the readings
        sensors.listofsoilStates[sensors.counterAllreadings] = sensors_getSoilState(&sensors); // Get the soil state based on the readings
        sensors.counterAllreadings++; // Increment the counter for all readings
        sensors.counter_readings = 0; // Reset counter for next readings
    }
    else if (max_in_second == true) 
    {
        max_in_second = false; // Reset counter if it exceeds the array size
    }
    else 
    {
        // empty else    
    }

    if (sensors.counterAllreadings >= 3) 
    {
        sensors.fullReadingProcess_Complete = true; 

        // Serial.println("Full reading process complete");
        // Serial.println("Soil states for each sensor:");
        // for (int i = 0; i < 3; i++) 
        // {
        //     Serial.print("state ");
        //     Serial.print(i + 1);
        //     Serial.print(": ");
        //     Serial.print(sensors.listofsoilStates[i]);   
        //     Serial.print(": Soil State :  ");
        //     Serial.println(sensors_getSoilStateString(sensors.listofsoilStates[i])); 
       
        // }
    }

}

void Task1sec(void *parameters) {
    while(tasksRunning ) {
    if (watering_ongoing == false && (watering == 0 || watering < 0)) 
    {

        

        if (motor_check_if_watering_needed() == true) 
        {
            Serial.println(" 3 days passed, watering is checked");
            if (motor_handle_watering() > 0)
            {
                watering = 2 * motor_handle_watering();
                // Handle watering logic
                watering_ongoing = true; // Set flag to indicate watering is ongoing
            }
            else
            {
                RTC_setBootCount((RTC_getBootCount(false) - 3 ),true); // delay the next check for 1 day
                watering = 0; // No watering needed
                watering_ongoing = false; // Reset flag when no watering is needed
                Serial.println("No watering needed at this time");
            }

            Serial.println("Watering started for " + String(watering) + " seconds / 2");
        } 

        if (measurment_ongoing == true)
        {
            main_reading_logic(); // Call the main reading logic function
        }


        currentTime = millis(); 
        timedifference = currentTime - operationStartTime; // Calculate time difference since operation start
        // Serial.println("currentTime: " + String(currentTime));
        // Serial.println("Operation start time: " + String(operationStartTime));
        // Serial.println("millis " + String(millis()));
        Serial.println("Timedifference: " + String(timedifference));
        //Serial.println("CounterofAllreading  " + String(sensors.counterAllreadings));


        // Check if operation time has exceeded
        if(timedifference >= OPERATION_TIME  ) {
            Serial.println("Operation time exceeded, going to sleep");
            sensors.fullReadingProcess_Complete = false;
            shouldEnterSleep = true; 
            // Code after goToSleep() won't execute as device enters deep sleep

            for (int i = 0; i < RTC_getReadingIndex()  ; i++) {
                
                Serial.println("ALL reading History " + String(i ) + ": " + String(RTC_get_Reading_History(i)));
            }    

        }

        //         // Send periodic updates via Bluetooth
        // if (sensors.fullReadingProcess_Complete) {
        //     SerialBT.println("Sensor Update:");
        //     SerialBT.printf("Time: %lu ms\n", currentTime);
        //     for (int i = 0; i < 3; i++) {
        //         SerialBT.printf("Sensor %d: %s\n", 
        //             i + 1, 
        //             sensors_getSoilStateString(sensors.listofsoilStates[i]));
        //     }
        // }
        
        // // Handle incoming Bluetooth messages
        // handleBluetoothMessages();
        
    }
       
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL); // Delete this task after completion
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Give time for serial to initialize
    
    sensors = sensors_init();
    delay(10); // Give time for sensor initialization
    Serial.println("Sensor initialization complete");

    //  // Initialize Bluetooth
    // SerialBT.begin(DEVICE_NAME);
    // Serial.println("Bluetooth Started. Device name: " + String(DEVICE_NAME));

    // Record start time
    shouldEnterSleep = false; // Initialize sleep flag
    timedifference = 0; // Reset time difference
    currentTime = 0; // Reset current time
    operationStartTime = millis();
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);

    // Check if this is a wake up from deep sleep
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("Woken up from deep sleep");
    } else {
        Serial.println("First boot");
        RTC_init(); // Initialize RTC data
    }

    RTC_incrementBootCount(); // Increment boot count


    pinMode(BUILTIN_LED, OUTPUT);
    //digitalWrite(BUILTIN_LED, HIGH);



    // Create tasks
    xTaskCreate(
        Task40ms,          // Task function
        "Task40ms",        // Task name
        2048,           // Stack size
        NULL,           // Parameters
        1,              // Priority
        &Task1Handle    // Task handle
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

void loop() {
 
    if (shouldEnterSleep == true) {
        goToSleep();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}

