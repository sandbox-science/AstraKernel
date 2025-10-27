#include "memory.h"
#include "panic.h"
#include "printf.h"

#include <stdatomic.h>

// Default alignment: at least pointer size; 16 is a good general default.
static const size_t KMALLOC_ALIGN = (16 < sizeof(void*) ? sizeof(void*) : 16);

static _Atomic uintptr_t heap_cur = 0; // atomic in case of future multi core
static uintptr_t heap_end         = 0;

static inline uintptr_t align_up_uintptr(uintptr_t x, size_t align)
{
    return (x + (align - 1)) & ~(uintptr_t)(align - 1);
}

void kmalloc_init(void *start, void *limit)
{
    const uintptr_t s = (uintptr_t)start;
    const uintptr_t l = (uintptr_t)limit;
    if (l <= s)
    {
        kernel_panic("kmalloc_init: invalid heap range");
    }
    
    const uintptr_t aligned_start = align_up_uintptr(s, KMALLOC_ALIGN);
    const uintptr_t aligned_end   = l & ~(KMALLOC_ALIGN - 1);

    if (aligned_end <= aligned_start)
    {
        kernel_panic("kmalloc_init: heap too small after alignment");
    }

    heap_cur = aligned_start;
    heap_end = aligned_end;

    puts("kmalloc init\n");
}

void *kmalloc(size_t size)
{
    if (size == 0 || heap_cur == 0 || heap_end == 0)
    {
        return NULL; // not initialized
    }

    // Round size up to alignment
    size = (size + (KMALLOC_ALIGN - 1)) & ~(size_t)(KMALLOC_ALIGN - 1);

    if (size > (size_t)(heap_end - heap_cur))
    {
        kernel_panic("kmalloc: out of memory");
    }

    void *ptr = (void *)heap_cur;
    heap_cur += (uintptr_t)size;

    return ptr;
}

size_t kmalloc_remaining(void)
{
    return (heap_end > heap_cur) ? (size_t)(heap_end - heap_cur) : 0;
}
