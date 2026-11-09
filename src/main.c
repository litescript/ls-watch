// SPDX-License-Identifier: MPL-2.0
/* main.c */
#include "display.h"
#include "gpio.h"
#include "font.h"
#include <stdint.h>

#define WHITE 0xFFFFu

volatile uint32_t initialized_value = 0xABADBABE;
volatile uint32_t zeroed_value;

int main(void) {
    // make sure our RAM contains what we want
    if (initialized_value != 0xABADBABE || zeroed_value != 0) {
        __asm__ volatile("bkpt #0");
    }

    display_init();
    // get crazy with the cheez-whiz
    display_clear(0x0000u); // black screen
    // FONT TIME!
    font_draw_string(20u, 40u, "HELLO WORLD", WHITE);
    font_draw_string(20u, 60u, "HENDERSON", 0x07E0u);
    font_draw_string(20u, 80u, "THE BOYS IN THE LAB", 0xFFE0u);

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
