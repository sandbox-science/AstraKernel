/**
 * @file panic.h
 * @brief Kernel panic and halt routines.
 *
 * This module provides mechanisms to halt the system or 
 * stop execution in response to critical errors.
 */
#pragma once
#include "errno.h"

/**
 * @brief Halt the CPU indefinitely.
 *
 * Disables interrupts and places the processor in an infinite 
 * low-power wait state.
 *
 * @note This function never returns.
 */
[[noreturn]] void kernel_halt(void);

/**
 * @brief Print a panic message with error code and halt the CPU.
 *
 * Used to stop the kernel when an unrecoverable error occurs.
 *
 * @param message A human-readable string describing the reason 
 *        for the panic.
 *
 * @note This function never returns.
 */
[[noreturn]] void kernel_panic(const char *message, kerror_t error_code);
