#include <Arduino.h>
#include <sensors.h>
#include <motor.h>



// main Variables
float listofSensorsReadings[50] = {}; // Array to store sensor readings
int averageIndex_readings[5] = {}; // Index for the next sensor reading
int counter_readings = 0; // Counter for the number of readings
bool readingComplete = false; // Flag to indicate if readings are complete
bool fullReadingProcess_Complete = false; // Counter for seconds 
uint8_t counterAllreadings = 0; // Counter for all readings

// Sleep parameters
const uint64_t OPERATION_TIME =   16500;  // 16.5 seconds in milliseconds
const uint64_t SLEEP_TIME =  60000000;      // 60 seconds in milliseconds
unsigned long operationStartTime = 0;
bool shouldEnterSleep = false;

// Task handles
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;
TaskHandle_t Task3Handle = NULL;
TaskHandle_t Task4Handle = NULL;
volatile bool tasksRunning = true;
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
    // Task2Handle = NULL;
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



void calculateAverage() {
    if (readingComplete == true && counter_readings > 0) {
        float sum = 0;
        for (int i = 0; i < counter_readings; i++) {
             Serial.println("Reading " + String(i) + ": " + String(listofSensorsReadings[i]));
            sum += listofSensorsReadings[i];
            if ((i+1) % 10 == 0 && i != 0) {
              averageIndex_readings[i / 10] = sum /  10; // Store average every 10 readings
             Serial.print("sum: ");
             Serial.println(sum);
              sum = 0; // Reset sum for the next average calculation 

            }
        }
        for (int i = 0; i < 5; i++) {
            Serial.print("Average reading for sensor ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(averageIndex_readings[i]);
        }
        counterAllreadings++;
    } else {
        Serial.println("No readings to average.");
    }
}

// Task functions
void Task100ms(void *parameters) {
    while(tasksRunning ) {
        //Serial.println("Task 1 running - 100ms");

        

        if ( counter_readings < 50 && counter_readings >= 0 && fullReadingProcess_Complete == false) {
            // Read sensor data and store it in the array
            listofSensorsReadings[counter_readings] = sensors_readADC();
            counter_readings++;
            readingComplete = false; // Reset flag after reading
            //Serial.println("counter_readings:  " + String(counter_readings));
        } else {
            // Reset counter if it exceeds the array size
            counter_readings = 0;
        }

        if (counter_readings == 50  )
        {
            
            readingComplete = true; // Set flag when readings are complete
            calculateAverage(); // Calculate the average of the readings
            counter_readings = 0; // Reset counter for next readings
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL); // Delete this task after completion
}

void Task2(void *parameters) {
    while(tasksRunning ) {
        Serial.println("Task 2 running - 200ms");
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void Task3(void *parameters) {
    while(tasksRunning ) {
        Serial.println("Task 3 running - 50ms");
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void Task1sec(void *parameters) {
    while(tasksRunning ) {
        unsigned long currentTime = millis(); 
        Serial.println("currentTime: " + String(currentTime));
        Serial.println("Operation start time: " + String(operationStartTime));
        Serial.println("millis " + String(millis()));
        if (counterAllreadings == 3) {
        
        fullReadingProcess_Complete = true; 
            
        }
        // Check if operation time has exceeded
        if(currentTime - operationStartTime >= OPERATION_TIME) {
            Serial.println("Operation time exceeded, going to sleep");
            fullReadingProcess_Complete = false;
            shouldEnterSleep = true; 
            // Code after goToSleep() won't execute as device enters deep sleep
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL); // Delete this task after completion
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Give time for serial to initialize
    shouldEnterSleep = false; // Initialize sleep flag
    sensors_init();



    // Record start time
    operationStartTime = millis();
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);
    // Check if this is a wake up from deep sleep
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if(wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("Woken up from deep sleep");
    } else {
        Serial.println("First boot");
    }

    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, HIGH);



    // Create tasks
    xTaskCreate(
        Task100ms,          // Task function
        "Task100ms",        // Task name
        2048,           // Stack size
        NULL,           // Parameters
        1,              // Priority
        &Task1Handle    // Task handle
    );

    // xTaskCreate(
    //     Task2,
    //     "Task2",
    //     2048,
    //     NULL,
    //     1,
    //     &Task2Handle
    // );

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
        1,
        &Task4Handle
    );
}

void loop() {
 
    if (shouldEnterSleep == true) {
        goToSleep();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}

