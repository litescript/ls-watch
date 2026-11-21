// SPDX-License-Identifier: MPL-2.0
/* main.c */
#include "display.h"
#include "font.h"
// #include "gpio.h"
#include "rtc.h"
#include <stdint.h>

#define WHITE 0xFFFFu
#define GREEN 0x07E0u
#define YELLOW 0xFFE0u
#define BLACK 0x0000u

#define CLOCK_X 20u
#define CLOCK_Y 20u
#define DISP_SCALE 2u

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

    ClockTime sys_clock = {0, 0, 0, 0};
    DisplayTime dis_clock = {0, 0, 0};
    font_draw_time(&sys_clock, CLOCK_X, CLOCK_Y, WHITE, DISP_SCALE);
    uint32_t mask = 0xFFFFFFu;

    // assuming the loop exits when we expect, we get here, to configure GPIO
    // gpio_config_output(15);
    // gpio_set(15);
    // gpio_config_input(13);

    // wait for spacebar, then send interrupt to close the emulator
    for (;;) {
        // int pin_result = gpio_read(13);
        // if (pin_result == 1) {
        //     __asm__ volatile("bkpt #0");
        // }
        uint32_t current_ticks = rtc_get_ticks();
        uint32_t elapsed_ticks = current_ticks - previous_ticks;
        elapsed_ticks &= mask;
        clock_update(&sys_clock, elapsed_ticks);
        previous_ticks = current_ticks;
        if (sys_clock.seconds != dis_clock.seconds) {
            display_fill_rect(CLOCK_X + (6 * (6 * DISP_SCALE)), CLOCK_Y,
                              2 * (6 * DISP_SCALE), 7 * DISP_SCALE, BLACK);
            font_draw_two_digits(CLOCK_X + (6 * (6 * DISP_SCALE)), CLOCK_Y,
                                 sys_clock.seconds, WHITE, DISP_SCALE);
            dis_clock.seconds = sys_clock.seconds;
        }
        if (sys_clock.minutes != dis_clock.minutes) {
            display_fill_rect(CLOCK_X + (3 * (6 * DISP_SCALE)), CLOCK_Y,
                              2 * (6 * DISP_SCALE), 7 * DISP_SCALE, BLACK);
            font_draw_two_digits(CLOCK_X + (3 * (6 * DISP_SCALE)), CLOCK_Y,
                                 sys_clock.minutes, WHITE, DISP_SCALE);
            dis_clock.minutes = sys_clock.minutes;
        }
        if (sys_clock.hours != dis_clock.hours) {
            display_fill_rect(CLOCK_X, CLOCK_Y, 2 * (6 * DISP_SCALE),
                              7 * DISP_SCALE, BLACK);
            font_draw_two_digits(CLOCK_X, CLOCK_Y, sys_clock.hours, WHITE,
                                 DISP_SCALE);
            dis_clock.hours = sys_clock.hours;
        }
    }
}
