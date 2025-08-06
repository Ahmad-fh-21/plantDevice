#include <Arduino.h>

// Task handles
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;
TaskHandle_t Task3Handle = NULL;
TaskHandle_t Task4Handle = NULL;




// Pin definitions
const int ADC_PIN = 34;  // GPIO34 for analog input
const int DIGITAL_OUT_PIN = 2;

// ADC variables
const int ADC_READINGS = 10;

// Function to read and average ADC values
float readADC() {
    // Configure ADC
    analogReadResolution(12);  // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);  // Set ADC attenuation for 3.3V range
    
    // Take multiple readings and average them
    int sum = 0;
    for(int i = 0; i < ADC_READINGS; i++) {
        sum += analogRead(ADC_PIN);
        delay(1);  // Small delay between readings
    }
    int adcValue = sum / ADC_READINGS;
    
    // Convert to voltage
    float voltage = (adcValue * 3.3) / 4095.0;
    return voltage;
}

// Function to toggle digital output
void toggleDigitalOutput(bool state) {
    digitalWrite(DIGITAL_OUT_PIN, state);
    Serial.print("Digital Output: ");
    Serial.println(state ? "HIGH" : "LOW");
}

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

    Serial.println("ADC and Digital Output Test");
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
    // Empty loop as tasks handle the work

        // Read and display ADC value
    float voltage = readADC();
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
    
    // Toggle digital output
    toggleDigitalOutput(HIGH);
    delay(1000);
    toggleDigitalOutput(LOW);
    delay(1000);
    vTaskDelay(pdMS_TO_TICKS(1000));
}