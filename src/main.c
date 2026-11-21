// SPDX-License-Identifier: MPL-2.0
/* main.c */
#include "display.h"
#include "font.h"
#include "gpio.h"
#include "rtc.h"
#include <stdint.h>

#define WHITE 0xFFFFu
#define GREEN 0x07E0u
#define YELLOW 0xFFE0u
#define BLACK 0x0000u

volatile uint32_t initialized_value = 0xABADBABE;
volatile uint32_t zeroed_value;

int main(void) {
    rtc_init();
    uint32_t previous_ticks = rtc_get_ticks();
    // make sure our RAM contains what we want
    if (initialized_value != 0xABADBABE || zeroed_value != 0) {
        __asm__ volatile("bkpt #0");
    }

    display_init();
    // get crazy with the cheez-whiz
    display_clear(BLACK); // black screen

    // ClockTime test = {
    //     .hours = 23u,
    //     .minutes = 59u,
    //     .seconds = 59u,
    //     .partial_ticks = 0u,
    // };
    // clock_update(&test, 8u);
    // font_draw_time(&test, 20u, 20u, WHITE, 2u);
    //
    // ClockTime test2 = {12, 45, 0, 5};
    // clock_update(&test2, 6u);
    // font_draw_time(&test2, 20u, 60u, WHITE, 2u);
    // font_draw_uint(20u, 90u, test2.partial_ticks, WHITE, 2u);

    ClockTime sys_clock = {0, 0, 0, 0};
    uint32_t mask = 0xFFFFFFu;

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
        uint32_t current_ticks = rtc_get_ticks();
        uint32_t elapsed_ticks = current_ticks - previous_ticks;
        uint8_t seconds_before = sys_clock.seconds;
        elapsed_ticks &= mask;
        clock_update(&sys_clock, elapsed_ticks);
        previous_ticks = current_ticks;
        uint8_t seconds_after = sys_clock.seconds;
        if ((seconds_after - seconds_before) != 0) {
            display_clear(BLACK);
            font_draw_time(&sys_clock, 20u, 20u, WHITE, 2u);
        }
    }
}
