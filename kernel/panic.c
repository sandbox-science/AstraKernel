#include "panic.h"
#include "printf.h"

/**
 * @brief Halt the CPU indefinitely.
 */
__attribute__((noreturn)) void kernel_halt(void)
{
    __asm__ volatile("cpsid i\n cpsid f\n"); // disable interrupts
    for (;;)
    {
        __asm__ volatile("wfi");    // wait for interrupt
    }
}

/**
 * @brief Print a panic message and halt the CPU
 *
 * @param message: the reason of raising a panic.
 *
 * @todo implement a custom error message with error codes ( errno.h/.c )
 *       ex: https://github.com/dthain/basekernel/blob/master/library/errno.c
 */
void kernel_panic(const char *message)
{
    printf("%s", message);
    kernel_halt();
}
