# Kernel Panic Subsystem

The panic subsystem provides the kernel with a controlled way
to stop execution when a fatal error occurs.

## Design
- Panics print an error message to the console before halting.
- The halt routine disables interrupts and executes `wfi`
  indefinitely, ensuring the CPU does not execute further
  instructions.

## Future Plans
- Add structured error codes via `errno.h/.c`.
- Provide hooks for logging the panic state (e.g., stack trace).
- Possibly support multi-core halting (broadcast halt signal).
