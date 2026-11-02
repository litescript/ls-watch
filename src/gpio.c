#include "gpio.h"
#include <stdint.h>

void gpio_set(uint32_t pin) {
    GPIO_OUTSET = GPIO_PIN_MASK(pin);
}

void gpio_clear(uint32_t pin) {
    GPIO_OUTCLR = GPIO_PIN_MASK(pin);
}

void gpio_config_output(uint32_t pin) {
    gpio_clear(pin);
    GPIO_PIN_CNF(pin) =
        GPIO_CNF_DIR_OUTPUT | GPIO_CNF_INPUT_DISCONNECT | GPIO_CNF_PULL_DISABLE;
}

void gpio_config_input(uint32_t pin) {
    GPIO_PIN_CNF(pin) =
        GPIO_CNF_DIR_INPUT | GPIO_CNF_INPUT_CONNECT | GPIO_CNF_PULL_DISABLE;
}

uint32_t gpio_read(uint32_t pin) {
    uint32_t result = GPIO_PIN_MASK(pin) & GPIO_IN;
    if (result != 0u) {
        return 1;
    } else {
        return 0;
    }
}
