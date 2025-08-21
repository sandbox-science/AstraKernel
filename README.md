# AstraKernel ~ A minimal ARM kernel for QEMU

AstraKernel is a minimal experimental kernel written in C and ARM assembly, designed to run on 
QEMU's VersatileAB (Cortex-A8) emulated platform. This project currently 
demonstrates how to bootstrap a system, set up the stack, zero the `.bss` 
section, and perform basic UART output (printing text to the serial console).

```bash
========================================
  AstraKernel  v0.1.0
  Built May 15 2025 at 22:58:58
========================================

  CPU: Cortex-A8 @ 200MHz (simulated)
  RAM: 128MB SDRAM at 0x00000000

Welcome to your own little Astra world!
Type away, explore, have fun.
```

## Features so far

- Simple ARM assembly startup code
- Memory-mapped UART output for terminal messages
- Runs under QEMU

## Building

### Native Build

Make sure you have an ARM cross-compiler installed (e.g., `arm-none-eabi-gcc`) and `qemu-system-arm`.

```sh
make
```

> [!IMPORTANT]
> 
> `make` will clean, build, and run the kernel in QEMU. You can also run 
`make qemu` to run the kernel without cleaning or building it again.

### Docker Build

If you have docker installed, you can also run AstraKernel through a docker container:

```sh
make docker
```

> [!IMPORTANT]
> 
> `make docker` will pull from the most recent `main` commit from the upstream repository
> `https://github.com/sanbox-science/AstraKernel.git`.
> If you wish to use a local copy, you can run `make docker-dev`, which will copy all
> local build files into the repository.

## Documentation

For more details about this kernel, refer to the [AstraKernel Documentation](https://github.com/sandbox-science/AstraKernel/blob/main/doc/AstraKernelManual.pdf).

> [!NOTE]
> 
> The manual is a work in progress and may not cover all features yet.

## License

This project is licensed under the GNU GENERAL PUBLIC License. See the [LICENSE](LICENSE) file for details.