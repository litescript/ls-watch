// SPDX-License-Identifier: MPL-2.0
/* rtc.c */
#include "rtc.h"
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
    RTC1_PRESCALER = 4095u; // 32768 / (4095 + 1) = 8 Hz
    RTC1_TASKS_CLEAR = 1u;
    RTC1_TASKS_START = 1u;
}

uint32_t rtc_get_ticks(void) {
    return RTC1_COUNTER;
}

void rtc_stop(void) {
    RTC1_TASKS_STOP = 1u;
}

void rtc_start(void) {
    RTC1_TASKS_START = 1u;
}

void rtc_clear(void) {
    RTC1_TASKS_CLEAR = 1u;
}

void clock_update(ClockTime *clock, uint32_t elapsed_ticks) {
    uint32_t total_ticks = clock->partial_ticks + elapsed_ticks;
    uint32_t remaining_ticks = total_ticks % 8u;

    uint32_t whole_seconds = total_ticks / 8u;
    uint32_t total_seconds = clock->seconds + whole_seconds;
    uint32_t whole_minutes = total_seconds / 60u;
    uint32_t remaining_seconds = total_seconds % 60u;

    uint32_t total_minutes = clock->minutes + whole_minutes;
    uint32_t whole_hours = total_minutes / 60u;
    uint32_t remaining_minutes = total_minutes % 60u;

    uint32_t total_hours = clock->hours + whole_hours;
    uint32_t remaining_hours = total_hours % 24u;

    clock->partial_ticks = remaining_ticks;
    clock->seconds = remaining_seconds;
    clock->minutes = remaining_minutes;
    clock->hours = remaining_hours;
}
