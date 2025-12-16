/**
 * @copyright Copyright (c) 2025 SandBox Science
 * @license GPL-3.0 license
 * @author Christopher Dedman Rollet <chrisdedman@proton.me>
 *
 * @file memory.c
 * @brief Simple kernel memory allocator (kmalloc/kfree) implementation.
 *
 * This file implements a basic dynamic memory allocator for the kernel,
 * providing functions to allocate and free memory blocks. The allocator
 * uses a linked list of memory block headers to manage free and used
 * memory regions within a predefined heap area.
 *
 * The implementation includes:
 * - `kmalloc_init()`: Initializes the heap region for dynamic allocation.
 * - `kmalloc()`: Allocates a block of memory of a specified size.
 * - `kfree()`: Frees a previously allocated block of memory.
 *
 * The allocator uses a first-fit strategy and merges adjacent free blocks
 * to reduce fragmentation.
 *
 * @note These implementations are inspired by
 *      https://github.com/dthain/basekernel/blob/master/kernel/kmalloc.c
 */
#include "memory.h"
#include "panic.h"
#include "errno.h"
#include <stdint.h>

static struct header *head = NULL;
/**< Default alignment: at least pointer size; 16 is a good general default. */
static const size_t KMALLOC_ALIGN = (16 < sizeof(void*) ? sizeof(void*) : 16); 

struct header *kmalloc_get_head(void)
{
    return head;
}

/**
 * @brief Align a pointer up to the next multiple of a given alignment.
 *
 * This function takes a pointer value `x` and aligns it up to the next
 * multiple of `align`. The `align` parameter must be a power of 2.
 *
 * @param x The pointer value to align.
 * @param align The alignment value (must be a power of 2).
 *
 * @return uintptr_t The aligned pointer value.
 */
static inline uintptr_t align_up_uintptr(uintptr_t x, size_t align)
{
    return (x + (align - 1)) & ~(uintptr_t)(align - 1);
}

void kmalloc_init(void *restrict start, void *restrict limit)
{
    const uintptr_t s = (uintptr_t)start;
    const uintptr_t l = (uintptr_t)limit;
    if (l <= s)
    {
        kernel_panic("kmalloc_init: invalid heap range", KERR_INVAL);
    }

    const uintptr_t aligned_start = align_up_uintptr(s, KMALLOC_ALIGN);
    const uintptr_t aligned_end   = l & ~(KMALLOC_ALIGN - 1);

    if ( (aligned_end - aligned_start) <= sizeof(struct header) )
    {
        kernel_panic("kmalloc_init: heap too small after alignment", KERR_NO_SPACE);
    }

    head  = (struct header *)aligned_start;
    *head = (struct header)
    {
        .size  = aligned_end - aligned_start - sizeof(struct header),
        .state = BLOCK_FREE,
        .next  = NULL,
        .prev  = NULL,
    };
}

/**
 * @internal
 * @brief Split a large free memory block into two parts if it is larger than the requested size.
 *
 * This function takes a pointer to a free memory block and divides it into
 * two smaller blocks if the block's size is greater than the requested size
 * plus the size needed for the allocator's bookkeeping structure.
 *
 * The first part of the block will be used to satisfy the allocation request,
 * while the second part remains free and is linked back into the free list.
 *
 * @param curr Pointer to the current free block to be split.
 * @param size Requested size in bytes for allocation.
 */
static void ksplit_block(struct header *curr, size_t size)
{
    struct header *new = (struct header *)((char *)curr + sizeof(struct header) + size);
    *new = (struct header)
    {
        .state = BLOCK_FREE,
        .size  = curr->size - size - sizeof(struct header),
        .prev  = curr,
        .next  = curr->next
    };

    if (curr->next)
    {
        curr->next->prev = new;
    }
    curr->next = new;
    curr->size = size;
}

void *kmalloc(size_t size)
{
    if (size == 0)
    {
        return NULL; // not initialized
    }

    // Round size up to alignment
    size = (size + (KMALLOC_ALIGN - 1)) & ~(size_t)(KMALLOC_ALIGN - 1);

    struct header *curr = head;
    while(curr != NULL)
    {
        if (curr->state != BLOCK_USED && curr->size >= size)
        {
            break;
        }
        curr = curr->next;
    }

    if (curr == NULL)
    {
        kernel_panic("kmalloc:", KERR_NOMEM);
    }

    if (curr->size >= size + sizeof(struct header) + KMALLOC_ALIGN)
    {
        ksplit_block(curr, size);
    }
    curr->state = BLOCK_USED;

    return (void*)((char*)curr + sizeof(struct header));
}

/**
 * @internal
 * @brief Merge a block with the its next free block.
 *
 * if the the block exist and both the current and next
 * block are free, merge the next block with the current block.
 * Keep merging while the next block is free.
 *
 * @param curr Pointer to the current free block.
 */
static void kmerge(struct header *curr)
{
    if (!curr || curr->state != BLOCK_FREE)
    {
        return;
    }

    while (curr->next && curr->next->state == BLOCK_FREE)
    {
        curr->size += sizeof(struct header) + curr->next->size;
        curr->next = curr->next->next;
        if (curr->next)
        {
            curr->next->prev = curr;
        }
    }
}

void kfree(void *block)
{
    if (!block)
    {
        return;
    }

    struct header *curr = (struct header*)((char*)block - sizeof(struct header));

    if (curr->state != BLOCK_USED)
    {
        kernel_panic("kfree:", KERR_INVAL);
    }

    curr->state = BLOCK_FREE;
    kmerge(curr);
    kmerge(curr->prev);
}
