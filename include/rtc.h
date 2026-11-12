// SPDX-License-Identifier: MPL-2.0
/* rtc.h */
#ifndef RTC_H
#define RTC_H
#include <stdint.h>

void rtc_init(void);
uint32_t rtc_get_ticks(void);

#endif
