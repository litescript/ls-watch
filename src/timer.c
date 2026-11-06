/* timer.c */
#include "timer.h"
#include <stdint.h>

void delay_ms(uint32_t ms) {
    SYST_RVR = 63999u;
    SYST_CVR = 0u;
    SYST_CSR = SYST_CSR_ENABLE | SYST_CSR_CLKSOURCE;

    for (uint32_t i = 0; i < ms; i++) {
        while ((SYST_CSR & SYST_CSR_COUNTFLAG) == 0u) {
            // wait for COUNTFLAG to become set
        }
    }

    SYST_CSR = 0u;
}
