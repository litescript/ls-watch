// SPDX-License-Identifier: MPL-2.0
/* gpio.h */
#ifndef LS_WATCH_GPIO_H
#define LS_WATCH_GPIO_H
#include <stdbool.h>
#include <stdint.h>

#define GPIO_BASE 0x50000000u
#define GPIO_OUTSET_ADDR (GPIO_BASE + 0x508u)
#define GPIO_OUTCLR_ADDR (GPIO_BASE + 0x50Cu)
#define GPIO_IN_ADDR (GPIO_BASE + 0x510u)
#define GPIO_DIRSET_ADDR (GPIO_BASE + 0x518u)
#define GPIO_DIRCLR_ADDR (GPIO_BASE + 0x51Cu)

#define GPIO_OUTSET (*(volatile uint32_t *)GPIO_OUTSET_ADDR)
#define GPIO_OUTCLR (*(volatile uint32_t *)GPIO_OUTCLR_ADDR)
#define GPIO_IN (*(volatile uint32_t *)GPIO_IN_ADDR)
#define GPIO_DIRSET (*(volatile uint32_t *)GPIO_DIRSET_ADDR)
#define GPIO_DIRCLR (*(volatile uint32_t *)GPIO_DIRCLR_ADDR)

#define GPIO_PIN_MASK(pin) (1u << (pin))
#define GPIO_PIN_CNF_ADDR(pin) (GPIO_BASE + 0x700u + (4u * (pin)))
#define GPIO_PIN_CNF(pin) (*(volatile uint32_t *)GPIO_PIN_CNF_ADDR(pin))

#define GPIO_CNF_DIR_OUTPUT (1u << 0)
#define GPIO_CNF_DIR_INPUT (0u << 0)

#define GPIO_CNF_INPUT_CONNECT (0u << 1)
#define GPIO_CNF_INPUT_DISCONNECT (1u << 1)

#define GPIO_CNF_PULL_UP (3u << 2)
#define GPIO_CNF_PULL_DISABLE (0u << 2)

void gpio_set(uint32_t pin);
void gpio_clear(uint32_t pin);
void gpio_config_output(uint32_t pin);
uint32_t gpio_read(uint32_t pin);
void gpio_config_input(uint32_t pin);
void gpio_config_output_initial(uint32_t pin, bool initial_high);

#endif
