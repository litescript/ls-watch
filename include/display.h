#ifndef LS_WATCH_DISPLAY_H
#define LS_WATCH_DISPLAY_H

#include <stdint.h>

void display_init_pins(void);
void display_write_command(uint8_t command);
void display_write_data(uint8_t data);
void display_hardware_reset(void);
void display_init(void);

#endif
