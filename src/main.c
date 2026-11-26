// SPDX-License-Identifier: MPL-2.0
/* main.c */
#include "battery.h"
#include "display.h"
#include "font.h"

#ifdef EMULATOR
#include "gpio.h"
#endif

#include "rtc.h"
#include <stdbool.h>
#include <stdint.h>

#define WHITE 0xFFFFu
#define GREEN 0x07E0u
#define YELLOW 0xFFE0u
#define BLACK 0x0000u

#define CLOCK_X 20u
#define CLOCK_Y 20u
#define DISP_SCALE 2u
#define BATTERY_X 100u
#define BATTERY_Y 100u
#define BATTERY_SCALE 3u

volatile uint32_t initialized_value = 0xABADBABE;
volatile uint32_t zeroed_value;

int main(void) {
    rtc_init();

#ifndef EMULATOR
    battery_init();
    uint8_t last_check_seconds = 99u;
    uint16_t battery_mv;
    uint16_t last_battery_mv = UINT16_MAX;
    bool battery_err_displayed = false;
#endif

    uint32_t previous_ticks = rtc_get_ticks();

#ifdef EMULATOR
    // make sure our RAM contains what we want
    if (initialized_value != 0xABADBABE || zeroed_value != 0) {
        __asm__ volatile("bkpt #0");
    }
#endif

    display_init();
    // get crazy with the cheez-whiz
    display_clear(BLACK); // black screen

    ClockTime sys_clock = {0, 0, 0, 0};
    DisplayTime dis_clock = {0, 0, 0};
    font_draw_time(&sys_clock, CLOCK_X, CLOCK_Y, WHITE, DISP_SCALE);
    uint32_t mask = 0xFFFFFFu;

// assuming the loop exits when we expect, we get here, to configure GPIO
#ifdef EMULATOR
    gpio_config_output(15);
    gpio_set(15);
    gpio_config_input(13);
#endif

    // wait for spacebar, then send interrupt to close the emulator
    for (;;) {
#ifdef EMULATOR
        int pin_result = gpio_read(13);
        if (pin_result == 1) {
            __asm__ volatile("bkpt #0");
        }
#endif

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

#ifndef EMULATOR
        if (sys_clock.seconds % 5 == 0u &&
            last_check_seconds != sys_clock.seconds) {
            last_check_seconds = sys_clock.seconds;
            battery_mv = averaged_battery_reading(8u);
            if (battery_mv == 0u) {
                if (!battery_err_displayed) {
                    display_fill_rect(BATTERY_X, BATTERY_Y,
                                      DISPLAY_WIDTH - BATTERY_X,
                                      7 * BATTERY_SCALE, BLACK);
                    font_draw_string(BATTERY_X, BATTERY_Y, "ERR", YELLOW, 3u);
                    battery_err_displayed = true;
                }
            } else {
                if (battery_err_displayed || battery_mv != last_battery_mv) {
                    display_fill_rect(BATTERY_X, BATTERY_Y,
                                      DISPLAY_WIDTH - BATTERY_X,
                                      7 * BATTERY_SCALE, BLACK);
                    font_draw_uint(BATTERY_X, BATTERY_Y, (uint32_t)battery_mv,
                                   YELLOW, BATTERY_SCALE);
                    battery_err_displayed = false;
                }
            }
            last_battery_mv = battery_mv;
        }
#endif
    }
}
