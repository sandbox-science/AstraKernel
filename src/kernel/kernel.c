#include "init.h"
#include "kernel_loop.h"

// Conditional include for debug functions if USE_KTESTS is defined.
// This assumes 'debug.h' provides the 'run_kernel_tests' function.
#ifdef USE_KTESTS
#include "debug.h"
#endif

/**
 * @brief The main entry point for the C part of the kernel.
 * 
 * This function orchestrates the kernel's startup sequence and main operational loop.
 * It first initializes all necessary kernel subsystems, then optionally runs
 * a suite of tests, and finally enters the main interactive loop.
 */
void kernel_main(void)
{
    // Step 3 & 6: Call the high-level initialization function.
    // This function encapsulates all initial setup, including screen clear,
    // memory manager setup, and welcome message display.
    kernel_init();

    // Step 7: Handle debugger/testing functions.
    // If USE_KTESTS is defined, an orchestrator function for running various
    // kernel tests (e.g., IRQ sanity, memory tests, timer tests) is called.
#ifdef USE_KTESTS
    run_kernel_tests();
#endif

    // Step 5 & 6: Call the main operational loop function.
    // This function encapsulates the kernel's primary execution flow,
    // including the command-line interface and the switch-case logic.
    kernel_run_main_loop();

    // If kernel_run_main_loop ever returns (e.g., if an exit command is processed
    // and the kernel is not designed to halt automatically), the kernel would
    // typically enter a final halt state here to prevent unintended execution.
    // For AstraKernel, exiting the main loop (via 'q') currently signifies
    // the end of interactive operation. A permanent halt might be added here
    // if the system is not intended to reset or power off.
    // while (true) { __asm__ volatile("wfi"); } // Example: Wait For Interrupt (halt)
}