// SPDX-License-Identifier: MPL-2.0
/* font.h */
#ifndef FONT_H
#define FONT_H

#include <stdint.h>

void font_draw_char(uint16_t x, uint16_t y, char character, uint16_t color);
void font_draw_string(uint16_t x, uint16_t y, const char *string, uint16_t color);

#endif
