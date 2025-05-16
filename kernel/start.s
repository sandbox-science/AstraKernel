    .section .text
    .global _start

_start:
    // 1. Set up the stack pointer
    ldr sp, =_estack // Load the stack pointer with the top of the stack
    bic sp, sp, #7

    // 2. Zero the .bss section
    ldr r0, =__bss_start   // Start address (symbol from linker script)
    ldr r1, =__bss_end     // End address (symbol from linker script)
    mov r2, #0             // init zero-value for BSS clearing

zero_bss:
    cmp r0, r1             // Compare current address to end
    bge bss_done           // If done, skip zeroing
    str r2, [r0], #4       // Store zero at [r0], increment r0 by 4
    b zero_bss

bss_done:
    // 3. Call kernel_main
    bl kernel_main

    // 4. Halt if kernel_main returns (should not happen)
hang:
    b hang		// Infinite loop