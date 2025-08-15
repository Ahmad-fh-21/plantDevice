#ifndef RTC_HANDLER_H
#define RTC_HANDLER_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_READINGS_INDEX 252 // Maximum number of readings


// Function declarations
//extern void rtc_init(void);

int RTC_getBootCount(bool old);
void RTC_setBootCount(int count, bool old );
int RTC_getReadingIndex();
void RTC_setReadingIndex(int index);
void RTC_clearReadings();
void RTC_init();
uint16_t RTC_get_Reading_History(uint8_t index); 
void RTC_saveReading( uint16_t soilState);
void RTC_incrementBootCount(void);


#ifdef __cplusplus
}
#endif

#endif // RTC_HANDLER_H