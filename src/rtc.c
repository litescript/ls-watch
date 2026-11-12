// SPDX-License-Identifier: MPL-2.0
/* rtc.c */
#include <stdint.h>

#define RTC1_BASE 0x40011000u
#define RTC1_TASKS_START (*(volatile uint32_t *)(RTC1_BASE + 0x000u))
#define RTC1_TASKS_STOP (*(volatile uint32_t *)(RTC1_BASE + 0x004u))
#define RTC1_TASKS_CLEAR (*(volatile uint32_t *)(RTC1_BASE + 0x008u))
#define RTC1_COUNTER (*(volatile uint32_t *)(RTC1_BASE + 0x504u))
#define RTC1_PRESCALER (*(volatile uint32_t *)(RTC1_BASE + 0x508u))

#define LFCLK_BASE 0x40000000u
#define LFCLK_TASKS_LFCLKSTART (*(volatile uint32_t *)(LFCLK_BASE + 0x008u))
#define LFCLK_EVENTS_LFCLKSTARTED (*(volatile uint32_t *)(LFCLK_BASE + 0x104u))
#define LFCLK_LFCLKSRC (*(volatile uint32_t *)(LFCLK_BASE + 0x518u))

void rtc_init(void) {
    LFCLK_LFCLKSRC = 0u;
    LFCLK_EVENTS_LFCLKSTARTED = 0u;
    LFCLK_TASKS_LFCLKSTART = 1u;

    while (LFCLK_EVENTS_LFCLKSTARTED == 0u) {
        // wait for clock to start
    }

    RTC1_TASKS_STOP = 1u;
    RTC1_PRESCALER = 0u; // not magic number, based on 32.768 Hz LFCLK / 8
    RTC1_TASKS_CLEAR = 1u;
    RTC1_TASKS_START = 1u;
}

uint32_t rtc_get_ticks(void) {
    return RTC1_COUNTER;
}
