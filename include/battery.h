// SPDX-License-Identifier: MPL-2.0
/* battery.h */
#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

void battery_init(void);
uint16_t battery_read_mv(void);

#endif
