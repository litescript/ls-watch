#include <stdint.h>

volatile uint32_t initialized_value = 0xABADBABE;
volatile uint32_t zeroed_value;

int main(void) {
    if (initialized_value != 0xABADBABE || zeroed_value != 0) {
        __asm__ volatile("bkpt #0");
    } else {
        for (;;) {
            __asm__ volatile("wfi");
        }
    }
    return 0xDEADBEEF;
}
