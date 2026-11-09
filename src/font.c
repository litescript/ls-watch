// SPDX-License-Identifier: MPL-2.0
/* font.c */
#include "font.h"
#include "display.h"

#include <stdint.h>
#include <stddef.h>

// big ass character struct
static const uint8_t font_5x7[26][7] = {
    // A
    {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001},
    // B
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110},
    // C
    {0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111},
    // D
    {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110},
    // E
    {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111},
    // F
    {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000},
    // G
    {0b01111, 0b10000, 0b10000, 0b10111, 0b10001, 0b10001, 0b01111},
    // H
    {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001},
    // I
    {0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},
    // J
    {0b00111, 0b00010, 0b00010, 0b00010, 0b10010, 0b10010, 0b01100},
    // K
    {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001},
    // L
    {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111},
    // M
    {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001},
    // N
    {0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001},
    // O
    {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110},
    // P
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000},
    // Q
    {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101},
    // R
    {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001},
    // S
    {0b01111, 0b10000, 0b10000, 0b01110, 0b00001, 0b00001, 0b11110},
    // T
    {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100},
    // U
    {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110},
    // V
    {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100},
    // W
    {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b10101, 0b01010},
    // X
    {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001},
    // Y
    {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100},
    // Z
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111},
};

void font_draw_char(uint16_t x, uint16_t y, char character, uint16_t color) {
    if (character < 'A' || character > 'Z') {
        return;
    }
    uint8_t glyph_index = (uint8_t)(character - 'A');

    for (uint8_t row = 0; row < 7; row++) {
        for (uint8_t column = 0; column < 5; column++) {
            uint8_t mask = (uint8_t)(1u << (4u - column));
            if (font_5x7[glyph_index][row] & mask) {
                display_draw_pixel(x + column, y + row, color);
            }
        }
    }
}

void font_draw_string(uint16_t x, uint16_t y, const char *string, uint16_t color) {
    size_t index = 0u;
    uint32_t adv = 0u;

    while (string[index] != '\0') {
        if ((uint32_t)x + adv > DISPLAY_WIDTH - 5u) {
            return;
        }

        font_draw_char(x + adv, y, string[index], color);
        adv += 6u;
        index++;
    }
}
