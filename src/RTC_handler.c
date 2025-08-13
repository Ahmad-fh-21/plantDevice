#include <RTC_handler.h>




// RTC data
RTC_DATA_ATTR int RTC_bootCount = 0;  // Preserved during deep sleep
RTC_DATA_ATTR uint16_t RTC_SoilStates[255] = {0};  // Save last readings
RTC_DATA_ATTR int RTC_readingIndex = 0;                  // Current index in the array



// Funtion to set and get RTC_bootCount
void RTC_setBootCount(int count) {
    RTC_bootCount = count;
}

int RTC_getBootCount() {
    return RTC_bootCount;
}

// Function to set and get RTC_readingIndex
void RTC_setReadingIndex(int index) {
    if (index >= 0 && index < 255) {
        RTC_readingIndex = index;
    }
}
int RTC_getReadingIndex() {
    return RTC_readingIndex;
}

void RTC_clearReadings() {
    for (int i = 0; i < 255; i++) {
        RTC_SoilStates[i] = 0; // Clear all readings
    }
    RTC_readingIndex = 0; // Reset index
}

void RTC_init() {
    // Initialize RTC data if needed
    RTC_setBootCount(0);
    RTC_clearReadings();
    RTC_setReadingIndex(0);
}

uint16_t RTC_get_Reading_History( uint8_t index) {
    if ( index < 255 && index >= 0) {
        return RTC_SoilStates[index];
    } else {
        RTC_setReadingIndex(0); // Reset index if out of bounds
        return 0; // Return 0 if index is out of bounds
    }
}
    
//  function to save readings into RTC memory
void RTC_saveReading(uint16_t soilState) {
    if (RTC_readingIndex < 255 && RTC_readingIndex >= 0) {
        RTC_SoilStates[RTC_readingIndex] = soilState;
        RTC_readingIndex++;
    }
}

