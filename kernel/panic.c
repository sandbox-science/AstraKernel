#include "panic.h"
#include "printf.h"

/**
 * @brief Halt the CPU indefinitely.
 */
[[noreturn]] void kernel_halt(void)
{
    __asm__ volatile(
        "cpsid i\n\t"
        "cpsid f\n\t"
        :
        :
        : "memory"  // this prevent reordering of memory operations
    ); // disable interrupts
    for (;;)
    {
        __asm__ volatile("wfi" ::: "memory");    // wait for interrupt
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
[[noreturn]] void kernel_panic(const char *message)
{
    printf("%s", message);
    kernel_halt();
}
