#include "display.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include <stdint.h>

#define ST7789_CMD_SLPOUT 0x11u
#define ST7789_CMD_DISPON 0x29u
#define ST7789_CMD_COLMOD 0x3Au
#define ST7789_PIXEL_FORMAT_RGB565 0x55u
#define ST7789_CMD_CASET 0x2Au
#define ST7789_CMD_RASET 0x2Bu
#define ST7789_CMD_RAMWR 0x2Cu

/*
 * PineTime ST7789 display control pins
 *
 * P0.18 - D/C:       LOW = command, HIGH = data
 * P0.25 - CS:        LOW = display selected, HIGH = deselected
 * P0.26 - RESET:     LOW = reset asserted, HIGH = normal operation
 * P0.23 - BACKLIGHT: LOW = on, HIGH = off
 *
 * All four pins are outputs from the nRF52832.
 */

void display_init_pins(void) {
    // configure display control pins as outputs
    gpio_config_output(18); // D/C
    gpio_config_output(25); // CS
    gpio_config_output(26); // RESET
    gpio_config_output(23); // BACKLIGHT

    // establish initial idle states
    gpio_set(25); // deselect display
    gpio_set(26); // release hardware reset
    gpio_set(23); // keep backlight off
}

void display_write_command(uint8_t command) {
    gpio_clear(18);     // D/C LOW: command follows
    spi_write(command); // transmit command byte
}

void display_write_data(uint8_t data) {
    gpio_set(18);    // D/C HIGH: data follows
    spi_write(data); // transmit data byte
}

void display_hardware_reset(void) {
    gpio_clear(26); // RESET LOW: hold ST7789 in hardware reset
    delay_ms(20);   // maintain reset for 20 ms
    gpio_set(26);   // RESET HIGH: release display from reset
    delay_ms(150);  // allow display controller to recover
}

void display_init(void) {
    // prepare communication and display control hardware
    display_init_pins();      // configure D/C, CS, RESET, and backlight GPIOs
    spi_init();               // initialize SPIM0 for comm with ST7789
    display_hardware_reset(); // reset display controller to a known state

    // wake the display controller
    display_begin();
    display_write_command(ST7789_CMD_SLPOUT); // 0x11: exit sleep mode
    display_end();
    delay_ms(150); // wait for sleep-out to complete

    // configure the pixel color format
    display_begin();
    display_write_command(ST7789_CMD_COLMOD);       // 0x3A: set pixel format
    display_write_data(ST7789_PIXEL_FORMAT_RGB565); // 0x55: 16 bits per pixel
    display_end();

    // enable the display and its illumination
    display_begin();
    display_write_command(ST7789_CMD_DISPON); // 0x29: turn the display output on
    display_end();
    gpio_clear(23); // BACKLIGHT LOW: enable illumination
}

void display_write_color(uint16_t color) {
    display_write_data((uint8_t)(color >> 8));
    display_write_data((uint8_t)(color & 0xFFu));
}

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    display_begin();
    display_write_command(ST7789_CMD_CASET);
    display_write_data((uint8_t)(x0 >> 8));    // high byte
    display_write_data((uint8_t)(x0 & 0xFFu)); // low byte
    display_write_data((uint8_t)(x1 >> 8));    // high byte
    display_write_data((uint8_t)(x1 & 0xFFu)); // low byte

    display_write_command(ST7789_CMD_RASET);
    display_write_data((uint8_t)(y0 >> 8));    // high byte
    display_write_data((uint8_t)(y0 & 0xFFu)); // low byte
    display_write_data((uint8_t)(y1 >> 8));    // high byte
    display_write_data((uint8_t)(y1 & 0xFFu)); // low byte
    display_end();
}

void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    display_set_window(x, y, x, y);
    display_begin();
    display_write_command(ST7789_CMD_RAMWR);
    display_write_color(color);
    display_end();
}

void display_begin(void) {
    gpio_clear(25); // CS LOW: select the display
}

void display_end(void) {
    gpio_set(25); // CS HIGH: deselect the display
}
