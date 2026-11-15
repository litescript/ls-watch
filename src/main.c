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

    delay_ms(1u);
    display_init();
    // get crazy with the cheez-whiz
    display_clear(0x0000u); // black screen

    ClockTime test = {
        .hours = 23u,
        .minutes = 59u,
        .seconds = 59u,
        .partial_ticks = 0u,
    };
    clock_update(&test, 8u);
    font_draw_time(&test, 20u, 20u, WHITE, 2u);

    ClockTime test2 = {12, 45, 0, 5};
    clock_update(&test2, 6u);
    font_draw_time(&test2, 20u, 60u, WHITE, 2u);
    font_draw_uint(20u, 90u, test2.partial_ticks, WHITE, 2u);

    // time keeping?
    // rtc_init();
    //
    // font_draw_string(20u, 70u, "ELAPSED:", YELLOW, 1);
    //
    // uint32_t before = rtc_get_ticks();
    // delay_ms(2000u);
    // uint32_t after = rtc_get_ticks();
    //
    // uint32_t elapsed = (after - before) & 0xFFFFFFu;
    //
    // font_draw_uint(20u, 100u, elapsed, WHITE, 1);

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
