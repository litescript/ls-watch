/* display.h */
#ifndef LS_WATCH_DISPLAY_H
#define LS_WATCH_DISPLAY_H

#define DISPLAY_WIDTH 240u
#define DISPLAY_HEIGHT 240u

#include <stdint.h>

void display_init_pins(void);
void display_write_command(uint8_t command);
void display_write_data(uint8_t data);
void display_hardware_reset(void);
void display_init(void);
void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void display_write_color(uint16_t color);
void display_begin(void);
void display_end(void);
void display_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void display_clear(uint16_t color);
void display_draw_hline(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void display_draw_vline(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void display_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

#endif
