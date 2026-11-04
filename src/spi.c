#include "spi.h"
#include "gpio.h"
#include <stdint.h>

void spi_init(void) {
    gpio_config_output(2);
    gpio_set(2);
    gpio_config_output(3);

    SPIM0_CONFIG = SPIM_CONFIG_CPHA | SPIM_CONFIG_CPOL;
    SPIM0_FREQUENCY = SPIM_FREQUENCY_1M;
    SPIM0_PSEL_SCK = 2u;
    SPIM0_PSEL_MOSI = 3u;
    SPIM0_RXD_MAXCNT = 0u;
    SPIM0_ENABLE = SPIM_ENABLE_VALUE;
}

void spi_write(uint8_t byte) {
    uint8_t tx_byte = byte;

    SPIM0_EVENTS_END = 0u;
    SPIM0_TXD_PTR = (uint32_t)(uintptr_t)&tx_byte;
    SPIM0_TXD_MAXCNT = 1u;
    SPIM0_TASKS_START = 1u;

    while (SPIM0_EVENTS_END == 0u) {
        // wait for EasyDMA to finish
    }
}
