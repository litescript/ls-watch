// SPDX-License-Identifier: MPL-2.0
/* main.c */
#include "display.h"
#include "font.h"
#include "gpio.h"
#include "rtc.h"
#include "timer.h"
#include <stdint.h>

#define WHITE 0xFFFFu
#define GREEN 0x07E0u
#define YELLOW 0xFFE0u

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
    font_draw_string(20u, 40u, "HELLO WORLD", WHITE, 1);
    font_draw_string(20u, 60u, "HENDERSON", GREEN, 3);
    font_draw_string(20u, 120u, "THE BOYS IN THE LAB", YELLOW, 1);
    font_draw_string(20u, 140u, "IT IS 12:45 PM", WHITE, 2);
    font_draw_string(20u, 180u, "DO YOU KNOW WHERE YOUR INTERN IS", YELLOW, 1);

    // time keeping?
    rtc_init();
    uint32_t t0 = rtc_get_ticks();
    delay_ms(2000);
    uint32_t t1 = rtc_get_ticks();
    delay_ms(2000);
    uint32_t t2 = rtc_get_ticks();

    font_draw_string(20u, 200u, "ELAPSED TICKS:", WHITE, 1u);
    font_draw_uint(20u, 220u, t1 - t0, WHITE, 2u);
    font_draw_uint(100u, 220u, t2 - t1, WHITE, 2u);

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
