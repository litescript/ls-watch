#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void Reset_Handler(void);
void Default_Handler(void);
extern int main(void);

__attribute__((used, section(".isr_vector")))
const uintptr_t vector_table[55] = {
    [0] = (uintptr_t)&_estack,
    [1] = (uintptr_t)Reset_Handler,
    [2 ... 54] = (uintptr_t)Default_Handler,
};

void Reset_Handler(void) {
  const uint32_t *src = &_sidata;
  uint32_t *dst = &_sdata;

  // copy flash to RAM to initialize .data
  while (dst < &_edata) {
    *dst++ = *src++;
  }

  // initialize .bss by zeroing it out explicitly
  uint32_t *bssStart = &_sbss;
  while (bssStart < &_ebss) {
    *bssStart++ = 0;
  }

  main();
  Default_Handler();
}

void Default_Handler(void) {
  for (;;) {
    __asm__ volatile("bkpt #0");
  }
}
