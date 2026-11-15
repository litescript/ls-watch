// SPDX-License-Identifier: MPL-2.0
/* rtc.h */
#ifndef RTC_H
#define RTC_H
#include <stdint.h>

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t partial_ticks;
} ClockTime;

void rtc_init(void);
uint32_t rtc_get_ticks(void);
void rtc_stop(void);
void rtc_start(void);
void rtc_clear(void);
void clock_update(ClockTime *clock, uint32_t elapsed_ticks);

#endif
