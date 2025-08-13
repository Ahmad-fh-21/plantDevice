#ifndef RTC_HANDLER_H
#define RTC_HANDLER_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif





// Function declarations
//extern void rtc_init(void);

int RTC_getBootCount();
void RTC_setBootCount(int count);
int RTC_getReadingIndex();
void RTC_setReadingIndex(int index);
void RTC_clearReadings();
void RTC_init();
uint16_t RTC_get_Reading_History(uint8_t index); 
void RTC_saveReading( uint16_t soilState);


#ifdef __cplusplus
}
#endif

#endif // RTC_HANDLER_H