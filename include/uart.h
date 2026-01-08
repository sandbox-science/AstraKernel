/**
 * @file uart.h
 * @brief UART0 register map for QEMU VersatileAB/PB.
 */
#pragma once

#include <stdint.h>

// Memory-mapped I/O registers for UART0 on QEMU VersatileAB/PB.
#define UART0_BASE  0x101F1000u
#define UART0_DR    (*(volatile uint32_t *)UART0_BASE)          // Data Register
#define UART0_FR    (*(volatile uint32_t *)(UART0_BASE + 0x18)) // Flag Register

// UART Flag Register bits.
#define UART_FR_TXFF (1u << 5) // Transmit FIFO full
#define UART_FR_RXFE (1u << 4) // Receive FIFO empty
