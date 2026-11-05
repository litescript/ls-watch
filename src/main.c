#include "display.h"
#include "gpio.h"
// #include "spi.h"
#include <stdint.h>

volatile uint32_t initialized_value = 0xABADBABE;
volatile uint32_t zeroed_value;

int main(void) {
    // make sure our RAM contains what we want
    if (initialized_value != 0xABADBABE || zeroed_value != 0) {
        __asm__ volatile("bkpt #0");
    }

    display_init();
    display_fill_rect(100u, 100u, 20u, 20u, 0xF800u);
    display_fill_rect(20u, 100u, 20u, 20u, 0x07E0u);
    display_fill_rect(100u, 20u, 20u, 20u, 0x001Fu);

    // assuming the loop exits when we expect, we get here, to configure GPIO
    gpio_config_output(15);
    gpio_set(15);
    gpio_config_input(13);

    // wait for spacebar, then send interrupt to close the emulator
    for (;;) {
        int pin_result = gpio_read(13);
        if (pin_result == 1) {
            __asm__ volatile("bkpt #0");
        }
    }
}
