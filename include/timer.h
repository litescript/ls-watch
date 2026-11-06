/* timer.h */
#ifndef LS_WATCH_TIMER_H
#define LS_WATCH_TIMER_H

#include <stdint.h>

#define SYST_CSR_ADDR 0xE000E010u
#define SYST_RVR_ADDR 0xE000E014u
#define SYST_CVR_ADDR 0xE000E018u

#define SYST_CSR (*(volatile uint32_t *)SYST_CSR_ADDR)
#define SYST_RVR (*(volatile uint32_t *)SYST_RVR_ADDR)
#define SYST_CVR (*(volatile uint32_t *)SYST_CVR_ADDR)

#define SYST_CSR_ENABLE (1u)
#define SYST_CSR_CLKSOURCE (1u << 2)
#define SYST_CSR_COUNTFLAG (1u << 16)

void delay_ms(uint32_t ms);

#endif
