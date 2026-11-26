// SPDX-License-Identifier: MPL-2.0
/* battery.h */
#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

typedef enum {
    BATTERY_OK,
    BATTERY_ERR_START_TIMEOUT,
    BATTERY_ERR_SAMPLE_TIMEOUT,
    BATTERY_ERR_STOP_TIMEOUT,
    BATTERY_ERR_INVALID_SAMPLE
} BatteryStatus;

void battery_init(void);
BatteryStatus get_battery_status(void);
uint16_t battery_read_mv(void);
uint16_t averaged_battery_reading(uint8_t times);

#endif
