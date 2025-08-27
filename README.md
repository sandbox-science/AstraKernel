# AstraKernel ~ A minimal ARM kernel for QEMU

[![GitHub release (including pre-releases)](https://img.shields.io/github/v/release/sandbox-science/AstraKernel?include_prereleases)](https://github.com/sandbox-science/AstraKernel/releases)

AstraKernel is a minimal experimental kernel written in modern C and ARM assembly, designed to run on
**QEMU's VersatileAB (Cortex-A8)** platform. The purpose is educational, showing the fundamental
steps of bringing up a bare-metal system, from low-level bootstrapping to higher-level interactive features
to explore kernel development concepts.

```bash
========================================
  AstraKernel  v0.1.0
  Built Aug 27 2025 at 12:56:35
========================================

  CPU: Cortex-A8 @ 200MHz (simulated)
  RAM: 128MB SDRAM at 0x00000000

Welcome to your own little Astra world!
Type away, explore, have fun.

AstraKernel is running...
Press Ctrl-A and then X to exit QEMU.

AstraKernel >
```

## Features so far

- [x] **Bootstrapping**: stack setup, BSS clearing, entry into the kernel main.
- [x] **UART I/O**: putc/puts, getc/getline, basic shell loop.
- [x] **Vector Table**: Define vector table for exceptions/interrupts.
- [x] **Basic Timer Interrupt**: Configure one hardware timer, hook its interrupt to increment a "tick" counter.

## Building

### Native Build

Make sure you have an ARM cross-compiler installed (e.g., `arm-none-eabi-gcc`) and `qemu-system-arm`.

```sh
make
```

Developers also have the option to run the kernel with custom flags, such as:
```sh
make KFLAGS="-USE_KTESTS -MY_DEFFLAG"
```

> [!IMPORTANT]
> 
> `make` will clean, build, and run the kernel in QEMU. You can also run 
`make qemu` to run the kernel without cleaning or building it again.

### Docker Build

If you have Docker installed, you can also run AstraKernel through a Docker container:

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

## Disclaimer

AstraKernel is an **educational project** created to demonstrate operating system concepts.
It is currently in version ![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/sandbox-science/AstraKernel?include_prereleases&label=&style=flat) and should be considered experimental software.

- **Not Production-Ready**: AstraKernel is not stable, secure, or suitable for production environments.
- **Educational Use Only**: The project is provided for learning, research, and experimentation purposes.
- **No Warranty**: AstraKernel is distributed on an “as is” basis, without warranties or conditions of any kind, express or implied.
- **Limitation of Liability**: The authors and contributors assume no responsibility or liability for any damages, data loss, or issues arising from the use of AstraKernel.

By using AstraKernel, you acknowledge that you understand these limitations.

## License

This project is licensed under the GNU GENERAL PUBLIC License. See the [LICENSE](LICENSE) file for details.
