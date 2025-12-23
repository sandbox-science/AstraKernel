#include "panic.h"
#include "printf.h"
//#include "errno.h"

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
 * @brief Print a panic message with error code and halt the CPU.
 *
 * Implementation details:
 * - Uses `printf()` to display the error message.
 * - Fetch the kernel error string based on the kernel error code.
 * - Calls ::kernel_halt() after printing.
 */
[[noreturn]] void kernel_panic(const char *message, kerror_t kerr_code)
{
    const char* code_str = kerr_is_err(kerr_code) ? error_str(kerr_code) : "no error code";
    printf("[PANIC] %s [%s]\n", message, code_str);
    kernel_halt();
}
