#include "panic.h"
#include "printf.h"

/**
 * @internal
 * @brief Halt the CPU indefinitely.
 *
 * Implementation details:
 * - Disables both IRQ (`cpsid i`) and FIQ (`cpsid f`) interrupts.
 * - Uses `wfi` (Wait For Interrupt) to place the CPU in a low-power
 *   idle loop.
 * - The `memory` clobber ensures no memory operations are reordered
 *   around this sequence.
 *
 * @see kernel_panic()
 */
[[noreturn]] void kernel_halt(void)
{
    __asm__ volatile(
        "cpsid i\n\t"
        "cpsid f\n\t"
        :
        :
        : "memory"
    );

    for (;;)
    {
        __asm__ volatile("wfi" ::: "memory");
    }
}

/**
 * @internal
 * @brief Print a panic message and halt the CPU.
 *
 * Implementation details:
 * - Currently uses `printf()` to display the error message.
 * - Calls ::kernel_halt() after printing.
 *
 * @todo Replace string-only message with structured error codes
 *       (see errno.h/.c).
 *       https://github.com/dthain/basekernel/blob/master/library/errno.c
 */
[[noreturn]] void kernel_panic(const char *message)
{
    printf("%s", message);
    kernel_halt();
}
