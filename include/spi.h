#ifndef LS_WATCH_SPI_H
#define LS_WATCH_SPI_H

#include <stdint.h>

#define SPIM0_BASE 0x40003000u

#define SPIM0_ENABLE_ADDR (SPIM0_BASE + 0x500u)
#define SPIM0_PSEL_SCK_ADDR (SPIM0_BASE + 0x508u)
#define SPIM0_PSEL_MOSI_ADDR (SPIM0_BASE + 0x50Cu)
#define SPIM0_FREQUENCY_ADDR (SPIM0_BASE + 0x524u)
#define SPIM0_CONFIG_ADDR (SPIM0_BASE + 0x554u)
#define SPIM0_TASKS_START_ADDR (SPIM0_BASE + 0x010u)
#define SPIM0_EVENTS_END_ADDR (SPIM0_BASE + 0x118u)
#define SPIM0_TXD_PTR_ADDR (SPIM0_BASE + 0x544u)
#define SPIM0_TXD_MAXCNT_ADDR (SPIM0_BASE + 0x548u)
#define SPIM0_RXD_MAXCNT_ADDR (SPIM0_BASE + 0x538u)

#define SPIM_CONFIG_CPHA (1u << 1)
#define SPIM_CONFIG_CPOL (1u << 2)
#define SPIM_FREQUENCY_1M 0x10000000u
#define SPIM_ENABLE_VALUE 7u

#define SPIM0_ENABLE (*(volatile uint32_t *)SPIM0_ENABLE_ADDR)
#define SPIM0_PSEL_SCK (*(volatile uint32_t *)SPIM0_PSEL_SCK_ADDR)
#define SPIM0_PSEL_MOSI (*(volatile uint32_t *)SPIM0_PSEL_MOSI_ADDR)
#define SPIM0_FREQUENCY (*(volatile uint32_t *)SPIM0_FREQUENCY_ADDR)
#define SPIM0_CONFIG (*(volatile uint32_t *)SPIM0_CONFIG_ADDR)
#define SPIM0_TASKS_START (*(volatile uint32_t *)SPIM0_TASKS_START_ADDR)
#define SPIM0_EVENTS_END (*(volatile uint32_t *)SPIM0_EVENTS_END_ADDR)
#define SPIM0_TXD_PTR (*(volatile uint32_t *)SPIM0_TXD_PTR_ADDR)
#define SPIM0_TXD_MAXCNT (*(volatile uint32_t *)SPIM0_TXD_MAXCNT_ADDR)
#define SPIM0_RXD_MAXCNT (*(volatile uint32_t *)SPIM0_RXD_MAXCNT_ADDR)

void spi_init(void);
void spi_write(uint8_t byte);

#endif
