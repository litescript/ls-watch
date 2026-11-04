#include "display.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include <stdint.h>

void display_init_pins(void) {
    gpio_config_output(18);
    gpio_config_output(25);
    gpio_config_output(26);
    gpio_config_output(23);

    gpio_set(25);
    gpio_set(26);
    gpio_set(23);
}

void display_write_command(uint8_t command) {
    gpio_clear(18);
    gpio_clear(25);
    spi_write(command);
    gpio_set(25);
}

void display_write_data(uint8_t data) {
    gpio_set(18);
    gpio_clear(25);
    spi_write(data);
    gpio_set(25);
}

void display_hardware_reset(void) {
    gpio_clear(26);
    delay_ms(20);
    gpio_set(26);
    delay_ms(150);
}

void display_init(void) {
    display_init_pins();
    spi_init();
    display_hardware_reset();
    display_write_command(0x11);
    delay_ms(150);
    display_write_command(0x29);
    gpio_clear(23);
}
