# AstraKernel

A minimal experimental kernel written in C and ARM assembly, designed to run on QEMU's VersatilePB (vexpress-a9) emulated platform. This project currently demonstrates how to bootstrap a system, set up the stack, zero the `.bss` section, and perform basic UART output (printing text to the serial console).

## Features so far

- Simple ARM assembly startup code
- Memory-mapped UART output for terminal messages
- No standard library dependencies
- Runs under QEMU

## File Overview

- **kernel/start.s**  
  ARM assembly startup code. Sets up the stack, zeros the `.bss` section, and jumps to the C kernel.
- **kernel/kernel.c**  
  Kernel entry point and basic I/O functions for UART communication.
- **kernel/kernel.ld**  
  Linker script to correctly map program sections in memory.

## How It Works

1. **Bootstrapping:**  
   The startup assembly (`start.s`) sets up the stack and clears uninitialized data.
2. **UART Output:**  
   The kernel maps UART registers as memory addresses. Simple C functions (`putc`, `puts`) send characters or strings to the UART (__string ain't working yet__), making text appear in the serial terminal.
3. **Main Loop:**  
   After printing a test message, the kernel loops forever, keeping the system alive.

## Building

Make sure you have an ARM cross-compiler installed (e.g., `arm-none-eabi-gcc`) and `qemu-system-arm`.

```sh
make
```

> [!IMPORTANT]
> 
> `make` will clean, build, and run the kernel in QEMU. You can also run `make qemu` to run the kernel without cleaning or building it again.