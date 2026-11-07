/* display.c */
#include "display.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include <stdbool.h>
#include <stdint.h>

#define ST7789_CMD_SLPOUT 0x11u
#define ST7789_CMD_DISPON 0x29u
#define ST7789_CMD_COLMOD 0x3Au
#define ST7789_PIXEL_FORMAT_RGB565 0x55u
#define ST7789_CMD_CASET 0x2Au
#define ST7789_CMD_RASET 0x2Bu
#define ST7789_CMD_RAMWR 0x2Cu

#define DC_PIN 18
#define CS_PIN 25
#define RESET_PIN 26
#define BACKLIGHT_PIN 23

#define DISPLAY_BUFFER_SIZE 128u

static volatile uint8_t pixel_buffer[DISPLAY_BUFFER_SIZE];

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

void display_begin(void) {
    gpio_clear(CS_PIN); // CS LOW: select the display
}

void display_end(void) {
    gpio_set(CS_PIN); // CS HIGH: deselect the display
}

void display_init_pins(void) {
    // configure display control pins as outputs
    gpio_config_output_initial(DC_PIN, false);       // D/C
    gpio_config_output_initial(CS_PIN, true);        // CS
    gpio_config_output_initial(RESET_PIN, true);     // RESET
    gpio_config_output_initial(BACKLIGHT_PIN, true); // BACKLIGHT
}

void display_write_command(uint8_t command) {
    gpio_clear(DC_PIN); // D/C LOW: command follows
    spi_write(command); // transmit command byte
}

void display_write_data(uint8_t data) {
    gpio_set(DC_PIN); // D/C HIGH: data follows
    spi_write(data);  // transmit data byte
}

void display_hardware_reset(void) {
    gpio_clear(RESET_PIN); // RESET LOW: hold ST7789 in hardware reset
    delay_ms(20);          // maintain reset for 20 ms
    gpio_set(RESET_PIN);   // RESET HIGH: release display from reset
    delay_ms(150);         // allow display controller to recover
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
    display_write_command(
        ST7789_CMD_DISPON); // 0x29: turn the display output on
    display_end();
    gpio_clear(BACKLIGHT_PIN); // BACKLIGHT LOW: enable illumination
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
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        return;
    }

    display_set_window(x, y, x, y);
    display_begin();
    display_write_command(ST7789_CMD_RAMWR);
    display_write_color(color);
    display_end();
}

void display_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                       uint16_t color) {
    if (width == 0u || height == 0u) {
        return;
    }
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        return;
    }

    uint16_t available_width = DISPLAY_WIDTH - x;
    uint16_t available_height = DISPLAY_HEIGHT - y;

    if (available_width < width) {
        width = available_width;
    }
    if (available_height < height) {
        height = available_height;
    }

    uint32_t area = (uint32_t)width * height;
    uint32_t bytes = area * 2u;

    // fill the buffer
    for (uint32_t i = 0; i < DISPLAY_BUFFER_SIZE / 2u; i++) {
        pixel_buffer[2 * i] = (uint8_t)(color >> 8);
        pixel_buffer[2 * i + 1] = (uint8_t)(color & 0xFFu);
    }

    // prep the display
    display_set_window(x, y, (x + width - 1u), (y + height - 1u));
    display_begin();
    display_write_command(ST7789_CMD_RAMWR);
    gpio_set(DC_PIN); // D/C HIGH: pixel data follows

    // transmit in chunks until complete
    while (bytes > 0u) {
        uint32_t chunk =
            (bytes >= DISPLAY_BUFFER_SIZE) ? DISPLAY_BUFFER_SIZE : bytes;

        spi_write_buffer(pixel_buffer, (uint16_t)chunk);
        bytes -= chunk;
    }

    display_end();
    // we're pretty much done here
}

void display_clear(uint16_t color) {
    display_fill_rect(0, 0, 240, 240, color);
}

void display_draw_hline(uint16_t x, uint16_t y, uint16_t length,
                        uint16_t color) {
    display_fill_rect(x, y, length, 1, color);
}

void display_draw_vline(uint16_t x, uint16_t y, uint16_t length,
                        uint16_t color) {
    display_fill_rect(x, y, 1, length, color);
}

void display_void_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                       uint16_t color) {
    if (width == 0 || height == 0) {
        return;
    }
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        return;
    }

    uint32_t right = (uint32_t)x + width - 1u;
    uint32_t bottom = (uint32_t)y + height - 1u;

    display_draw_hline(x, y, width, color);
    if (bottom < DISPLAY_HEIGHT) {
        display_draw_hline(x, (uint16_t)bottom, width, color);
    }
    display_draw_vline(x, y, height, color);
    if (right < DISPLAY_WIDTH) {
        display_draw_vline((uint16_t)right, y, height, color);
    }
}

void display_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                       uint16_t color) {
    int32_t x = x0;
    int32_t y = y0;
    int32_t dx = (x1 >= x0) ? (int32_t)x1 - x0 : (int32_t)x0 - x1;
    int32_t dy = (y1 >= y0) ? (int32_t)y1 - y0 : (int32_t)y0 - y1;
    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t error = dx - dy;

    for (;;) {
        display_draw_pixel((uint16_t)x, (uint16_t)y, color);
        if (x == x1 && y == y1) {
            break;
        }
        int32_t e2 = 2 * error;
        if (e2 > -dy) {
            error -= dy;
            x += sx;
        }
        if (e2 < dx) {
            error += dx;
            y += sy;
        }
    }
}

static void display_plot_checked(int32_t x, int32_t y, uint16_t color) {
    if (x < 0 || y < 0 || x >= (int32_t)DISPLAY_WIDTH ||
        y >= (int32_t)DISPLAY_HEIGHT) {
        return;
    }

    display_draw_pixel((uint16_t)x, (uint16_t)y, color);
}

static void display_circle_points(int32_t cx, int32_t cy, int32_t x, int32_t y,
                                  uint16_t color) {
    display_plot_checked(cx + x, cy + y, color);
    display_plot_checked(cx - x, cy + y, color);
    display_plot_checked(cx + x, cy - y, color);
    display_plot_checked(cx - x, cy - y, color);
    display_plot_checked(cx + y, cy + x, color);
    display_plot_checked(cx - y, cy + x, color);
    display_plot_checked(cx + y, cy - x, color);
    display_plot_checked(cx - y, cy - x, color);
}

void display_void_circle(uint16_t cx, uint16_t cy, uint16_t radius,
                         uint16_t color) {
    int32_t x = 0;
    int32_t y = radius;
    int32_t decision = 1 - (int32_t)radius;

    while (x <= y) {
        display_circle_points(cx, cy, x, y, color);
        if (decision < 0) {
            decision += 2 * x + 3;
        } else {
            decision += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

static void display_cline_checked(int32_t x, int32_t y, int32_t length,
                                  uint16_t color) {
    if (length <= 0 || y < 0 || x >= (int32_t)DISPLAY_WIDTH ||
        y >= (int32_t)DISPLAY_HEIGHT) {
        return;
    }
    if (x < 0) {
        length += x;
        x = 0;
    }
    if (x + length > (int32_t)DISPLAY_WIDTH) {
        length = (int32_t)DISPLAY_WIDTH - x;
    }
    if (length <= 0) {
        return;
    }

    display_draw_hline(x, y, length, color);
}

static void display_cfill_lines(int32_t cx, int32_t cy, int32_t x, int32_t y,
                                uint16_t color) {
    display_cline_checked(cx - x, cy + y, 2 * x + 1, color);
    display_cline_checked(cx - x, cy - y, 2 * x + 1, color);
    display_cline_checked(cx - y, cy + x, 2 * y + 1, color);
    display_cline_checked(cx - y, cy - x, 2 * y + 1, color);
}

void display_fill_circle(uint16_t cx, uint16_t cy, uint16_t radius,
                         uint16_t color) {
    int32_t x = 0;
    int32_t y = radius;
    int32_t decision = 1 - (int32_t)radius;

    while (x <= y) {
        display_cfill_lines(cx, cy, x, y, color);

        if (decision < 0) {
            decision += 2 * x + 3;
        } else {
            decision += 2 * (x - y) + 5;
            y--;
        }

        x++;
    }
}
