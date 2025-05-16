    .section .text
    .global _start

_start:
    // 1. Set up the stack pointer
    ldr sp, =0x00800000
    bic sp, sp, #7

    // 2. Zero the .bss section
    mov r2, #0             // init zero-value for BSS clearing

    ldr r0, =__bss_start   // Start address (symbol from linker script)
    ldr r1, =__bss_end     // End address (symbol from linker script)

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

    .align 8	// Reserve stack space
