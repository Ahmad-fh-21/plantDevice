#include <Arduino.h>
#include <sensors.h>
// Task handles
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;
TaskHandle_t Task3Handle = NULL;
TaskHandle_t Task4Handle = NULL;

// ****************************************************** 2.5 sec fill 100 mili liter 


// Task functions
void Task1(void *parameters) {
    while(1) {
        Serial.println("Task 1 running - 100ms");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Task2(void *parameters) {
    while(1) {
        Serial.println("Task 2 running - 200ms");
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void Task3(void *parameters) {
    while(1) {
        Serial.println("Task 3 running - 50ms");
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void Task4(void *parameters) {
    while(1) {
        Serial.println("Task 4 running - 100ms");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Give time for serial to initialize


    // // Create tasks
    // xTaskCreate(
    //     Task1,          // Task function
    //     "Task1",        // Task name
    //     2048,           // Stack size
    //     NULL,           // Parameters
    //     1,              // Priority
    //     &Task1Handle    // Task handle
    // );

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

    // xTaskCreate(
    //     Task4,
    //     "Task4",
    //     2048,
    //     NULL,
    //     1,
    //     &Task4Handle
    // );
}

void loop() {
 
    // Main loop does nothing, tasks run independently
    vTaskDelay(pdMS_TO_TICKS(1000)); // Just to keep the loop running
}

