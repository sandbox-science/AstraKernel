/* start.s — ARMv7-A (Cortex-A8), ARM state (A32) */

    .syntax unified
    .cpu    cortex-a8
    .arch   armv7-a

/* ------------------------------------------------------------- */
/* Exception Vector Table                                        */
/* ------------------------------------------------------------- */
    .section .vectors, "ax", %progbits
    .align 5                      // 32 bytes alignment (8 * 4B)

vectors_base:
    .global _start
    .type   _start, %function

/* 0x00 Reset        */   B   _start
/* 0x04 Undefined    */   B   undef_handler
/* 0x08 SWI/SVC      */   B   svc_entry
/* 0x0C PrefetchAbt  */   B   pabort_handler
/* 0x10 DataAbt      */   B   dabort_handler
/* 0x14 Reserved     */   B   reserved_handler
/* 0x18 IRQ          */   B   irq_entry
/* 0x1C FIQ          */   B   fiq_handler

/* ------------------------------------------------------------- */
/* Reset: Startup Code Section                                   */
/* ------------------------------------------------------------- */
    .section .text.startup, "ax", %progbits
    .align 4

    .extern kernel_main
    .extern __bss_start
    .extern __bss_end
    .extern __stack_top__
_start:
    // Enable Supervisor (SVC) mode explicitly and mask IRQ/FIQ
    MRS     R0, cpsr            // R0 <- CPSR (current status)
    BIC     R0, R0, #0x1F       // Clear Mode Bits (M[4:0])
    ORR     R0, R0, #0x13       // Set mode = SVC (0b10011)
    ORR     R0, R0, #(1 << 7)   // Set I bit (mask IRQ)
    ORR     R0, R0, #(1 << 6)   // Set F bit (mask FIQ)
    MSR     cpsr_c, R0          // Write CPSR control field

    // Program VBAR to the vector table base (low vectors),
    // to force low vector mode (needed for MMU)
    // ref: https://developer.arm.com/documentation/ddi0406/b/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/CP15-registers-for-a-VMSA-implementation/c12--Vector-Base-Address-Register--VBAR-
    LDR     R0, =vectors_base       // 32B aligned base of .vectors
    MCR     P15, 0, R0, C12, C0, 0  // Write  CP15 Vector Base Address Register (VBAR)
    ISB                             // This ensure new base is used for exception fetches

    // Ensure high vectors are disabled: SCTLR.V = 0
    MRC     P15, 0, R1, C1, C0, 0       // Read CP15 VBAR
    BIC     R1, R1, #(1 << 13)          // Clear SCTLR.V (high vectors off)
    MCR     P15, 0, R1, C1, C0, 0       // Write R1 to SCTLR
    ISB

    // Set SVC stack pointer (top of the RAM)
    LDR     sp, =__stack_top__
    BIC     sp, sp, #7            // Align to 8 bytes

    // Switch to IRQ mode by init its own stack
    MRS     R0, cpsr              // Save current CPSR
    BIC     R1, R0, #0x1F         // Clear mode bits
    ORR     R1, R1, #0x12         // Set mode = IRQ (0b10010)
    ORR     R1, R1, #(1 << 7)     // Keep IRQ masked
    MSR     cpsr_c, R1            // Switch to IRQ mode

    LDR     sp, =irq_stack_top    // Give IRQ mode its own stack
    BIC     sp, sp, #7

    // Back to SVC with IRQ/FIQ still masked
    MRS     R1, cpsr               // R1 <- CPSR in IRQ mode
    BIC     R1, R1, #0x1F
    ORR     R1, R1, #0x13          // Set mode = SVC
    ORR     R1, R1, #(1 << 7)      // I = 1 (IRQ)
    ORR     R1, R1, #(1 << 6)      // F = 1 (FIQ)
    MSR     cpsr_c, R1             // Back to SVC
    
    .extern __data_load
    .extern __data_start
    .extern __data_end

    LDR     R0, =__data_load
    LDR     R1, =__data_start
    LDR     R2, =__data_end
1:
    CMP     R1, R2
    BGE     2f
    LDR     R3, [R0], #4
    STR     R3, [R1], #4
    B       1b
2:

    // Zero init the .bss section
    LDR     R0, =__bss_start
    LDR     R1, =__bss_end
    MOV     R2, #0

zero_bss:
    CMP     R0, R1          // this check if we reach the end
    BGE     bss_done        // if start >= end -> done, else continue
    STR     R2, [R0], #4    // *R0 = 0; R0 += 4
    B       zero_bss        // Loop back until you reach the end
bss_done:
    BL      kernel_main
hang:
    B       hang        // Halt if kernel_main returns (shouldn't happen)

// Reserve space for IRQ mode stack
    .section .bss
    .align 8

irq_stack:
    .space 1024          // 1 KB stack for IRQ mode
irq_stack_top:

/* ------------------------------------------------------------- */
/* Exception Entries                                             */
/* ------------------------------------------------------------- */
    .section .text, "ax", %progbits
    .align 4

    .global svc_entry
    .type   svc_entry, %function
    .extern puts
svc_entry:
    STMDB   sp!, {R0-R3, R12, LR}   // save caller-saves register
    LDR     R0, =svc_msg
    BL      puts
    LDMIA   sp!, {R0-R3, R12, LR}
    SUBS    pc, LR, #0              // return from SVC (restores CPSR)

    .global irq_entry
    .type   irq_entry, %function
    .extern irq_handler
irq_entry:
    STMDB   sp!, {R0-R3, R12, LR}
    BL      irq_handler
    LDMIA   sp!, {R0-R3, R12, LR}
    SUBS    pc, LR, #4              // return from IRQ

/* ------------------------------------------------------------- */
/* Default handlers (spin until implemented)                     */
/* ------------------------------------------------------------- */
undef_handler:     B   hang
pabort_handler:    B   hang
dabort_handler:    B   hang
reserved_handler:  B   hang
fiq_handler:       B   hang

/* ------------------------------------------------------------- */
/* Read Only Data Section                                        */
/* ------------------------------------------------------------- */
    .section .rodata
svc_msg:
    .asciz "SVC Exception Handled!\r\n"

