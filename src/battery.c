// SPDX-License-Identifier: MPL-2.0
/* battery.c */
#include "battery.h"
#include "rtc.h"
#include <stdbool.h>
#include <stdint.h>

#define SAADC_BASE 0x40007000u
#define SAADC_TASKS_START                                                      \
    (*(volatile uint32_t *)(SAADC_BASE + 0x000u)) // prepare ADC and DMA
#define SAADC_TASKS_SAMPLE                                                     \
    (*(volatile uint32_t *)(SAADC_BASE + 0x004u)) // take a measurement
#define SAADC_TASKS_STOP                                                       \
    (*(volatile uint32_t *)(SAADC_BASE + 0x008u)) // stop the ADC
#define SAADC_EVENTS_STARTED                                                   \
    (*(volatile uint32_t *)(SAADC_BASE + 0x100u)) // ADC ready for sampling
#define SAADC_EVENTS_END                                                       \
    (*(volatile uint32_t *)(SAADC_BASE + 0x104u)) // result buffer is filled
#define SAADC_EVENTS_STOPPED                                                   \
    (*(volatile uint32_t *)(SAADC_BASE + 0x114u)) // to verify peripheral has
                                                  // actually stopped
#define SAADC_CH0_PSELP                                                        \
    (*(volatile uint32_t *)(SAADC_BASE + 0x510u)) // select analog input
#define SAADC_CH0_CONFIG                                                       \
    (*(volatile uint32_t *)(SAADC_BASE +                                       \
                            0x518u)) // reference, gain, acquisition time
#define SAADC_RESOLUTION                                                       \
    (*(volatile uint32_t *)(SAADC_BASE + 0x5F0u)) // set ADC resolution
#define SAADC_ENABLE                                                           \
    (*(volatile uint32_t *)(SAADC_BASE + 0x500u)) // enable peripheral
#define SAADC_RESULT_PTR                                                       \
    (*(volatile uint32_t *)(SAADC_BASE + 0x62Cu)) // RAM destination address
#define SAADC_RESULT_MAXCNT                                                    \
    (*(volatile uint32_t *)(SAADC_BASE + 0x630u)) // number of 16-bit samples

#define SAADC_CONFIG_TACQ_POS 16u    // we want value 5u, 40 microseconds
#define SAADC_CONFIG_GAIN_POS 24u;   // we want value 0u, 1/6
#define SAADC_CONFIG_REFSEL_POS 12u; // we want value 0u, internal reference
#define SAADC_CONFIG_MODE_POS 20u;   // we want value 0u, single-ended

static int16_t battery_sample;
static bool saadc_healthy = true;

void battery_init(void) {
    SAADC_CH0_PSELP = 8u;
    SAADC_CH0_CONFIG = 5u << SAADC_CONFIG_TACQ_POS;
    SAADC_RESOLUTION = 2u;
    SAADC_ENABLE = 1u;
    SAADC_RESULT_PTR = (uintptr_t)&battery_sample;
    SAADC_RESULT_MAXCNT = 1u; // collect 1 sample
}

static bool saadc_stop(void) {
    SAADC_EVENTS_STOPPED = 0u;
    uint32_t current_ticks = rtc_get_ticks();
    SAADC_TASKS_STOP = 1u;
    while (SAADC_EVENTS_STOPPED == 0u) {
        uint32_t elapsed = (rtc_get_ticks() - current_ticks) & 0xFFFFFFu;
        if (elapsed >= 8u) {
            break;
        }
    }
    if (SAADC_EVENTS_STOPPED == 0u) {
        saadc_healthy = false;
        return false;
    }

    return true;
}

uint16_t battery_read_mv(void) {
    if (!saadc_healthy) {
        return 0;
    }

    SAADC_EVENTS_STARTED = 0u;

    uint32_t current_ticks = rtc_get_ticks();

    SAADC_TASKS_START = 1u;

    while (SAADC_EVENTS_STARTED == 0u) {
        // wait for hardware
        uint32_t elapsed = (rtc_get_ticks() - current_ticks) & 0xFFFFFFu;
        if (elapsed >= 8u) {
            break;
        }
    }

    if (SAADC_EVENTS_STARTED == 0u) {
        bool abort = saadc_stop();
        if (!abort) {
            // uh oh
        }
        return 0;
    }

    SAADC_EVENTS_END = 0u;
    current_ticks = rtc_get_ticks();
    SAADC_TASKS_SAMPLE = 1u;

    while (SAADC_EVENTS_END == 0u) {
        // once again wait for hardware
        uint32_t elapsed = (rtc_get_ticks() - current_ticks) & 0xFFFFFFu;
        if (elapsed >= 8u) {
            break;
        }
    }

    if (SAADC_EVENTS_END == 0u) {
        bool abort = saadc_stop();
        if (!abort) {
            // uh oh
        }
        return 0;
    }

    bool stopped = saadc_stop();
    if (!stopped) {
        return 0;
    }

    if (battery_sample <= 0) {
        return 0;
    }

    int32_t battery_mv_reading = ((int32_t)battery_sample * 7200) / 4096;
    return (uint16_t)battery_mv_reading;
}
