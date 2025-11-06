# Memory Management

The memory management subsystem is responsible for handling the allocation,
deallocation, and organization of memory within the kernel. It ensures that
memory is used efficiently and safely, preventing leaks and corruption.

This subsystem provides a simple kernel heap allocator (`kmalloc`/`kfree`)
and manages memory blocks using a linked list of headers.

## Design

- Implements a **first-fit heap allocator** for dynamic memory allocation.
- Provides **public API functions**:
  - `kmalloc_init()` – Initialize the heap region.
  - `kmalloc()` – Allocate a memory block.
  - `kfree()` – Free a previously allocated block.
  - `kmalloc_get_head()` – Retrieve the head of the heap (mainly for testing).
- Uses **linked list of headers** to track free and used memory blocks.
- Performs **block splitting and merging** to reduce fragmentation.
- Aligns allocations to `KMALLOC_ALIGN` for proper memory access.

## Usage Example

```c
#include "memory.h"

extern char heap_start[];
extern char heap_end[];

void kernel_main(void)
{
    // Initialize the kernel heap
    kmalloc_init(heap_start, heap_end);

    // Allocate 128 bytes
    void *ptr = kmalloc(128);
    if (!ptr)
    {
        // Handle allocation failure
    }

    // Use memory
    char *data = (char*)ptr;
    data[0]    = 'X';

    // Free the memory
    kfree(ptr);
}
```
> **Note**: Always initialize the heap before using `kmalloc`.

## Future Plans
- Implement thread-safe operations for concurrent memory access.
- Integrate virtual memory support for better isolation and protection.
- Implement memory paging and swapping mechanisms.
- Add support for memory-mapped files and devices.
- Provide user-space memory management interfaces.
