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

#define SAADC_CONFIG_TACQ_POS 16u   // we want value 5u, 40 microseconds
#define SAADC_CONFIG_GAIN_POS 8u    // we want value 0u, 1/6
#define SAADC_CONFIG_REFSEL_POS 12u // we want value 0u, internal reference
#define SAADC_CONFIG_MODE_POS 20u   // we want value 0u, single-ended

// plausible battery mV bounds
#define BATTERY_MIN_VALID_MV 2500
#define BATTERY_MAX_VALID_MV 4300

static int16_t battery_sample;
static BatteryStatus battery_status = BATTERY_OK;

void battery_init(void) {
    SAADC_CH0_PSELP = 8u;
    SAADC_CH0_CONFIG = 5u << SAADC_CONFIG_TACQ_POS;
    SAADC_RESOLUTION = 2u;
    SAADC_ENABLE = 1u;
    SAADC_RESULT_PTR = (uintptr_t)&battery_sample;
    SAADC_RESULT_MAXCNT = 1u; // collect 1 sample
    battery_status = BATTERY_OK;
}

BatteryStatus get_battery_status(void) {
    return battery_status;
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
        battery_status = BATTERY_ERR_STOP_TIMEOUT;
        return false;
    }

    return true;
}

uint16_t battery_read_mv(void) {
    if (battery_status == BATTERY_ERR_STOP_TIMEOUT) {
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
        battery_status = BATTERY_ERR_START_TIMEOUT;
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
        battery_status = BATTERY_ERR_SAMPLE_TIMEOUT;
        bool abort = saadc_stop();
        if (!abort) {
            // uh oh
        }
        return 0;
    }

    bool stopped = saadc_stop();
    if (!stopped) {
        // uh oh
        return 0;
    }

    if (battery_sample <= 0) {
        battery_status = BATTERY_ERR_INVALID_SAMPLE;
        return 0;
    }

    int32_t battery_mv_reading = ((int32_t)battery_sample * 7200) / 4096;
    if (battery_mv_reading > BATTERY_MAX_VALID_MV ||
        battery_mv_reading < BATTERY_MIN_VALID_MV) {
        battery_status = BATTERY_ERR_INVALID_SAMPLE;
        return 0;
    }
    battery_status = BATTERY_OK;
    return (uint16_t)battery_mv_reading;
}

uint16_t averaged_battery_reading(uint8_t times) {
    if (times == 0u) {
        return 0u;
    }
    uint32_t reading_total = 0u;
    uint16_t failed_readings = 0u;
    for (uint16_t i = 0; i < times; i++) {
        uint16_t curr_reading;
        if ((curr_reading = battery_read_mv()) > 0) {
            reading_total += curr_reading;
        } else {
            failed_readings++;
        }
        if (get_battery_status() == BATTERY_ERR_STOP_TIMEOUT) {
            return 0;
        }
    }
    return (times > failed_readings)
               ? (uint16_t)(reading_total / (times - failed_readings))
               : 0u;
}
