/* main.c */
#include "display.h"
#include "gpio.h"
#include <stdint.h>

volatile uint32_t initialized_value = 0xABADBABE;
volatile uint32_t zeroed_value;

int main(void) {
    // make sure our RAM contains what we want
    if (initialized_value != 0xABADBABE || zeroed_value != 0) {
        __asm__ volatile("bkpt #0");
    }

    display_init();
    // get crazy with the cheez-whiz
    display_clear(0x001Fu);
    display_fill_rect(100u, 100u, 20u, 20u, 0xF800u);
    display_fill_rect(20u, 100u, 20u, 20u, 0x07E0u);
    display_fill_rect(100u, 20u, 20u, 20u, 0x0C3Cu);
    // edge tests
    display_fill_rect(230u, 230u, 30u, 30u, 0xFFE0u);
    display_fill_rect(250u, 250u, 20u, 20u, 0xFFFFu);
    // simple line test
    display_draw_line(20u, 20u, 200u, 100u, 0xFFFFu);
    // complex line test
    display_draw_line(120u, 120u, 220u, 120u, 0xFFFFu); // right
    display_draw_line(120u, 120u, 220u, 220u, 0xFFFFu); // down-right
    display_draw_line(120u, 120u, 120u, 220u, 0xFFFFu); // down
    display_draw_line(120u, 120u, 20u, 220u, 0xFFFFu);  // down-left
    display_draw_line(120u, 120u, 20u, 120u, 0xFFFFu);  // left
    display_draw_line(120u, 120u, 20u, 20u, 0xFFFFu);   // up-left
    display_draw_line(120u, 120u, 120u, 20u, 0xFFFFu);  // up
    display_draw_line(120u, 120u, 220u, 20u, 0xFFFFu);  // up-right
    // steep line test
    display_draw_line(30u, 30u, 60u, 210u, 0xFFE0u);
    display_draw_line(210u, 30u, 180u, 210u, 0x07FFu);
    // empty rectangle
    display_void_rect(40u, 40u, 100u, 40u, 0xCCCCu);
    display_void_rect(140u, 110u, 20u, 80u, 0x0000u);
    display_void_rect(10u, 220u, 20u, 180u, 0x0000u);
    display_void_rect(210u, 220u, 200u, 180u, 0x0000u);
    // circle time
    display_void_circle(180u, 180u, 55u, 0xFC0Au);
    display_void_circle(10u, 10u, 55u, 0xFC0Au);
    display_void_circle(180u, 180u, 100u, 0xFC0Au);
    display_fill_circle(120u, 120u, 70u, 0xF800u);

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
