# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).  
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Implemented a structured kernel error code via `errno.h`.
- Updated `kernel_panic` function to incorporate the new error code message.
- Doxygen documentation for memory management and kernel functions.
- Tests directory added to source build, with memory test functions moved to their own test file.
- Custom division operation added to kernel.

### Changed
- Moved Doxygen documentation from implementation files to header files.
- Removed Doxygen entries from public API; removed internal testing functions.
- Refactored memory allocation/deallocation: `kmalloc`, `kfree`, linked‐list allocator replacing earlier linear allocator.
- Consolidated IRQ handling: moved irq files into interrupt file and updated testing / handlers accordingly.
- Refactored `printf` handling and its testing; replaced `printf` calls with standardized types (via `<stdint.h>`).
- Replaced tracking of `map_file.map` and `/docs/html` in repository.

### Removed
- Old documentation excluded from Doxygen build.
- Tracked files: `map_file.map`, `/docs/html` directory.

## [0.1.0] – 2025‑08‑26
### Added
- Basic timer interrupt support.
- Vector table and exception handlers for IRQ & SVC.
- CPU migration from ARM926EJ to Cortex‑A8.
- Added `strcmp()` and `strlen()` functions, with API documentation.
- Added UART driver for character and string output; `printf()` functionality with examples.
- Real‑time clock support for datetime functions.

### Changed
- Documentation overhaul: updated `AstraKernelManual.pdf`, README improvements, formatting refinements.
- Refactor command handling in `kernel_main`.
- Refactor I/O and string modules for readability and maintainability.
- Introduced sanity‐check script for kernel ELF validation.
- Refactored linker script and initialization routines (e.g., stack pointer, BSS section).
- Source directory renamed from `src/` to `kernel/`, Makefile updated accordingly.

### Fixed
- Backspace logic fixed in `getlines()`.
- Minor typos in machine name and documentation comments.

### Merged
- Feature branches: `printf`, `datetime`, bugfix for `getlines()`, and others.

