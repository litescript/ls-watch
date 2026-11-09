// SPDX-License-Identifier: MPL-2.0
/* font.c */
#include "font.h"
#include <stdint.h>

static const uint8_t font_5x7[26][7] = {
    // A
    {0b01110, 0b10001, 0b10001, 0b11111,
    0b10001, 0b10001, 0b10001},

    // B
    {0b11110, 0b10001, 0b10001, 0b11110,
    0b10001, 0b10001, 0b11110},
}

static void font_draw_A(uint16_t x, uint16_t y, uint16_t color) {
    // implement
}
