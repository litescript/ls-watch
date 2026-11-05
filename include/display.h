#ifndef LS_WATCH_DISPLAY_H
#define LS_WATCH_DISPLAY_H

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

#endif
