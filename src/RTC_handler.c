#include <RTC_handler.h>




// RTC data
RTC_DATA_ATTR uint16_t RTC_bootCount = 0;  // Preserved during deep sleep
RTC_DATA_ATTR uint16_t RTC_bootCountOld = 0; // Previous boot count
RTC_DATA_ATTR uint16_t RTC_SoilStates[MAX_READINGS_INDEX] = {0};  // Save last readings
RTC_DATA_ATTR int RTC_readingIndex = 0;                  // Current index in the array


void RTC_init() {
    // Initialize RTC data if needed
    RTC_setBootCount(0, false); // Reset boot count
    RTC_setBootCount(0, true); // Save old boot count
    RTC_clearReadings();
    RTC_setReadingIndex(0);
}

void RTC_incrementBootCount(void) {
    if (RTC_bootCount < 65535) 
    { // Maximum value for uint16_t
        RTC_bootCount++;
    } else
    {
        RTC_bootCount = 0; // Reset boot count if it exceeds 255
    }
}


// Funtion to set and get RTC_bootCount
void RTC_setBootCount(int count, bool old ) 
{
    if (old) 
    {
        RTC_bootCountOld = count; // Save the old boot count
    }
    else 
    {
        RTC_bootCount = count;
    }
}

int RTC_getBootCount(bool old) {
    if (old) 
    {
        return RTC_bootCountOld; // Return the old boot count
    }
    else 
    {
        return RTC_bootCount; // Return the current boot count
    }

}

// Function to set and get RTC_readingIndex
void RTC_setReadingIndex(int index) {
    if (index >= 0 && index < MAX_READINGS_INDEX) 
    {
        RTC_readingIndex = index;
    }
}
int RTC_getReadingIndex() {
    return RTC_readingIndex;
}

void RTC_clearReadings() {
    for (int i = 0; i < MAX_READINGS_INDEX; i++) 
    {
        RTC_SoilStates[i] = 0; // Clear all readings
    }
    RTC_readingIndex = 0; // Reset index
}



uint16_t RTC_get_Reading_History( uint8_t index) {
    if ( index < MAX_READINGS_INDEX && index >= 0) 
    {
        return RTC_SoilStates[index];
    } else {
        //RTC_setReadingIndex(0); // Reset index if out of bounds
        return 0; // Return 0 if index is out of bounds
    }
}
    
//  function to save readings into RTC memory
void RTC_saveReading(uint16_t soilState) 
{
    if (RTC_readingIndex < MAX_READINGS_INDEX && RTC_readingIndex >= 0) 
    {
        RTC_SoilStates[RTC_readingIndex] = soilState;
        RTC_readingIndex++;
    }
    else if (RTC_readingIndex >= MAX_READINGS_INDEX) 
    {
        RTC_readingIndex = 0; // Reset index if it exceeds the maximum
        RTC_SoilStates[RTC_readingIndex] = soilState; // Save the new reading at index 0
    }
    else 
    {
        // Handle error: index out of bounds
       
    }
}

