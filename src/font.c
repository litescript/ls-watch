// SPDX-License-Identifier: MPL-2.0
/* font.c */
#include "font.h"
#include "display.h"

#include <stddef.h>
#include <stdint.h>

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

// numerals 0-9
static const uint8_t font_digits_5x7[10][7] = {
    // 0
    {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110},

    // 1
    {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110},

    // 2
    {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b11111},

    // 3
    {0b11110, 0b00001, 0b00001, 0b01110, 0b00001, 0b00001, 0b11110},

    // 4
    {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010},

    // 5
    {0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b00001, 0b11110},

    // 6
    {0b01110, 0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110},

    // 7
    {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b01000, 0b01000},

    // 8
    {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110},

    // 9
    {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110},
};

// :
static const uint8_t font_colon_5x7[7] = {
    0b00000, 0b00100, 0b00100, 0b00000, 0b00100, 0b00100, 0b00000,
};

static void text_putc(TextCursor *cursor, char c) {
    font_draw_char((uint16_t)cursor->x, (uint16_t)cursor->y, c, cursor->color,
                   cursor->scale);

    cursor->x += 6u * cursor->scale;
}

static void printdec(TextCursor *cursor, uint32_t number) {
    char digits[10];
    uint8_t count = 0;

    if (number == 0u) {
        text_putc(cursor, '0');
        return;
    }

    while (number > 0u) {
        digits[count++] = (char)('0' + (number % 10u));
        number /= 10u;
    }

    while (count > 0u) {
        text_putc(cursor, digits[--count]);
    }
}

// static void text_puts(TextCursor *cursor, const char *string) {
//     while (*string != '\0') {
//         text_putc(cursor, *string);
//         string++;
//     }
// }

void font_draw_uint(uint16_t x, uint16_t y, uint32_t number, uint16_t color,
                    uint8_t scale) {
    if (scale == 0u) {
        return;
    }

    TextCursor cursor = {
        .x = x,
        .y = y,
        .start_x = x,
        .color = color,
        .scale = scale,
    };

    printdec(&cursor, number);
}

void font_draw_char(uint16_t x, uint16_t y, char character, uint16_t color,
                    uint8_t scale) {
    if (scale == 0) {
        return;
    }

    const uint8_t *glyph = NULL;

    if (character >= 'A' && character <= 'Z') {
        glyph = font_5x7[character - 'A'];
    } else if (character >= '0' && character <= '9') {
        glyph = font_digits_5x7[character - '0'];
    } else if (character == ':') {
        glyph = font_colon_5x7;
    } else {
        return;
    }

    for (uint8_t row = 0; row < 7; row++) {
        for (uint8_t column = 0; column < 5; column++) {
            uint8_t mask = (uint8_t)(1u << (4u - column));
            if (glyph[row] & mask) {
                display_fill_rect(x + column * scale, y + row * scale, scale,
                                  scale, color);
            }
        }
    }
}

void font_draw_string(uint16_t x, uint16_t y, const char *string,
                      uint16_t color, uint8_t scale) {
    if (scale == 0) {
        return;
    }
    size_t index = 0u;
    uint32_t adv = 0u;

    while (string[index] != '\0') {
        if ((uint32_t)x + adv > DISPLAY_WIDTH - 5u) {
            return;
        }

        font_draw_char(x + adv, y, string[index], color, scale);
        adv += 6u * scale;
        index++;
    }
}
