// SPDX-License-Identifier: MPL-2.0
/* spi.c */
#include "spi.h"
#include "gpio.h"
#include <stddef.h>
#include <stdint.h>

#define SPIM0_MAX_TRANSFER 255u

void spi_init(void) {
    gpio_config_output_initial(2u, true);  // SCK: idle HIGH (SPI mode 3)
    gpio_config_output_initial(3u, false); // MOSI: initially LOW

    SPIM0_CONFIG = SPIM_CONFIG_CPHA | SPIM_CONFIG_CPOL;
    SPIM0_FREQUENCY = SPIM_FREQUENCY_1M;
    SPIM0_PSEL_SCK = 2u;
    SPIM0_PSEL_MOSI = 3u;
    SPIM0_RXD_MAXCNT = 0u;
    SPIM0_ENABLE = SPIM_ENABLE_VALUE;
}

void spi_write(uint8_t byte) {
    volatile uint8_t tx_byte = byte; // this must be volatile or things disappear

    SPIM0_EVENTS_END = 0u;
    SPIM0_TXD_PTR = (uint32_t)(uintptr_t)&tx_byte;
    SPIM0_TXD_MAXCNT = 1u;
    SPIM0_TASKS_START = 1u;

    while (SPIM0_EVENTS_END == 0u) {
        // wait for EasyDMA to finish
    }
}

void spi_write_buffer(volatile const uint8_t *data, uint16_t length) {
    if (data == NULL || length == 0u) {
        return;
    }

    uint16_t remaining = length;

    while (remaining > 0) {
        uint16_t chunk = (remaining >= SPIM0_MAX_TRANSFER) ? SPIM0_MAX_TRANSFER : remaining;
        SPIM0_EVENTS_END = 0u;
        SPIM0_TXD_PTR = (uint32_t)(uintptr_t)data;
        SPIM0_TXD_MAXCNT = chunk;
        SPIM0_TASKS_START = 1u;

        while (SPIM0_EVENTS_END == 0u) {
            // wait for transfer to finish
        }
        data += chunk;
        remaining -= chunk;
    }
}
