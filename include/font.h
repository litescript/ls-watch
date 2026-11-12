// SPDX-License-Identifier: MPL-2.0
/* font.h */
#ifndef FONT_H
#define FONT_H

#include <stdint.h>

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t start_x;
    uint16_t color;
    uint8_t scale;
} TextCursor;

void font_draw_char(uint16_t x, uint16_t y, char character, uint16_t color,
                    uint8_t scale);
void font_draw_string(uint16_t x, uint16_t y, const char *string,
                      uint16_t color, uint8_t scale);
void font_draw_uint(uint16_t x, uint16_t y, uint32_t number, uint16_t color,
                    uint8_t scale);

#endif
