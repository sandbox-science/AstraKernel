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
#include "printf.h"

static struct header *head = NULL;
/**< Default alignment: at least pointer size; 16 is a good general default. */
static const size_t KMALLOC_ALIGN = (16 < sizeof(void*) ? sizeof(void*) : 16); 

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

/**
 * @brief Initialize the kernel heap for dynamic memory allocation.
 *
 * This function sets up the initial heap region for `kmalloc`. It aligns
 * the start and end addresses according to the kernel alignment
 * requirements and initializes the first free memory block.
 *
 * @param start Pointer to the beginning of the heap memory region.
 *              Must be less than `limit`.
 * @param limit Pointer to the end of the heap memory region.
 *              Must be greater than `start`.
 *
 * @note If the heap range is invalid or too small, this function
 *       will call `kernel_panic()`.
 *
 * @note The pointers `start` and `limit` are marked `restrict` to indicate
 *       that their memory regions do not overlap, allowing the compiler
 *       to optimize pointer arithmetic safely.
 */
void kmalloc_init(void *restrict start, void *restrict limit)
{
    const uintptr_t s = (uintptr_t)start;
    const uintptr_t l = (uintptr_t)limit;
    if (l <= s)
    {
        kernel_panic("kmalloc_init: invalid heap range");
    }

    const uintptr_t aligned_start = align_up_uintptr(s, KMALLOC_ALIGN);
    const uintptr_t aligned_end   = l & ~(KMALLOC_ALIGN - 1);

    if ( (aligned_end - aligned_start) <= sizeof(struct header) )
    {
        kernel_panic("kmalloc_init: heap too small after alignment");
    }

    head  = (struct header *)aligned_start;
    *head = (struct header)
    {
        .size  = aligned_end - aligned_start - sizeof(struct header),
        .state = BLOCK_FREE,
        .next  = NULL,
        .prev  = NULL,
    };

    puts("kmalloc init\n");
}

/**
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

/**
 * @brief Allocate a block of memory from the kernel heap.
 * 
 * This function searches the heap for the first free block that is large
 * enough to satisfy the requested `size`. If the block is larger than needed,
 * it is split into an allocated block and a new free block.
 * @param size The number of bytes to allocate. Must be > 0.
 *
 * @return Pointer to the usable memory area of the allocated block, or
 *         NULL if `size` is zero.
 *
 * @note If no suitable block is found, the function will call `kernel_panic`.
 * @note The returned pointer points immediately after the block header.
 *
 */
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
        kernel_panic("kmalloc: out of memory");
    }

    if (curr->size >= size + sizeof(struct header) + KMALLOC_ALIGN)
    {
        ksplit_block(curr, size);
    }
    curr->state = BLOCK_USED;

    return (void*)((char*)curr + sizeof(struct header));
}

struct header *kmalloc_get_head(void)
{
    return head;
}

/**
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

/**
 * @brief Free a previously allocated block of memory.
 *
 * Marks the block as free and attempts to merge it with adjacent free blocks
 * to reduce fragmentation.
 *
 * @param block Pointer to the memory previously returned by `kmalloc`.
 *              Must not be NULL.
 *
 * @note If `block` is NULL, the function does nothing.
 * @note If `block` does not correspond to a valid allocated block,
 *       the function calls `kernel_panic`.
 */
void kfree(void *block)
{
    if (!block)
    {
        return;
    }

    struct header *curr = (struct header*)((char*)block - sizeof(struct header));

    if (curr->state != BLOCK_USED)
    {
        kernel_panic("kfree: invalid kfree");
    }

    curr->state = BLOCK_FREE;
    kmerge(curr);
    kmerge(curr->prev);
}


// --------------------------
// kmalloc/kfree tests
// --------------------------

#define TEST_HEAP_SIZE (1024 * 1024)
static uint8_t heap_space[TEST_HEAP_SIZE];
static size_t initial_heap_size = 0;

// --- Setup and teardown ---
static void setup()
{
    kmalloc_init(heap_space, heap_space + sizeof(heap_space));
    initial_heap_size = kmalloc_get_head()->size;
}

static void tear_down()
{
}

// --- Basic alloc/free correctness ---
static int kmalloc_test_single_alloc()
{
    void *ptr = kmalloc(128);
    if (!ptr)
    {
        printf("kmalloc returned NULL\n");
        return 0;
    }
    struct header *head = kmalloc_get_head();
    if (head->state != BLOCK_USED)
    {
        printf("Block not marked as used after kmalloc\n");
        return 0;
    }
    return 1;
}

static int kmalloc_test_single_alloc_and_free()
{
    void *ptr = kmalloc(128);
    kfree(ptr);
    struct header *head = kmalloc_get_head();
    if (head->state != BLOCK_FREE)
    {
        printf("Block not free after kfree\n");
        return 0;
    }
    if (head->size != initial_heap_size)
    {
        printf("Block size incorrect after kfree: got %lu expected %lu\n", head->size, initial_heap_size);
        return 0;
    }
    return 1;
}

static int kmalloc_test_merge_free_blocks()
{
    void *a = kmalloc(128);
    void *b = kmalloc(128);
    kfree(a);
    kfree(b);
    struct header *head = kmalloc_get_head();
    if (head->state != BLOCK_FREE)
    {
        printf("Head not free after merging\n");
        return 0;
    }
    if (head->size != initial_heap_size)
    {
        printf("Merged size incorrect: got %lu expected %lu\n", head->size, initial_heap_size);
        return 0;
    }
    return 1;
}

// --- Extended kfree edge case tests ---
// static int kfree_null_pointer_test()
// {
//     struct header *before = kmalloc_get_head();
//     kfree(NULL);
//     struct header *after = kmalloc_get_head();
//     if (panic_called)
//     {
//         printf("PANIC triggered on kfree(NULL)\n");
//         return 0;
//     }
//     if (before != after)
//         return 0;
//     return 1;
// }

// static int kfree_double_free_test()
// {
//     void *ptr = kmalloc(128);
//     kfree(ptr);
//     kfree(ptr); // second free should panic
//     if (!panic_called)
//     {
//         printf("Expected panic on double free, but none occurred\n");
//         return 0;
//     }
//     return 1;
// }

// static int kfree_invalid_pointer_inside_heap_test()
// {
//     void *ptr = kmalloc(128);
//     uint8_t *invalid = (uint8_t *)ptr + 8;
//     kfree(invalid);
//     if (!panic_called)
//     {
//         printf("Expected panic on invalid pointer inside heap\n");
//         return 0;
//     }
//     return 1;
// }

// static int kfree_invalid_pointer_outside_heap_test()
// {
//     uint8_t *invalid = heap_space - 32; // outside heap
//     kfree(invalid);
//     if (!panic_called)
//     {
//         printf("Expected panic on invalid pointer outside heap\n");
//         return 0;
//     }
//     return 1;
// }

static int kfree_merge_order_test()
{
    void *a = kmalloc(128);
    void *b = kmalloc(128);
    void *c = kmalloc(128);

    // Free middle first, then ends
    kfree(b);
    kfree(a);
    kfree(c);

    struct header *head = kmalloc_get_head();
    if (head->state != BLOCK_FREE)
    {
        printf("Heap not free after out-of-order merges\n");
        return 0;
    }
    if (head->size != initial_heap_size)
    {
        printf("Heap size incorrect after out-of-order merge: got %lu expected %lu\n", head->size, initial_heap_size);
        return 0;
    }
    return 1;
}

// --- Main test runner ---
int kmalloc_test()
{
    printf("Running kmalloc tests...\n");

    int (*tests[])(void) = {
        kmalloc_test_single_alloc,
        kmalloc_test_single_alloc_and_free,
        kmalloc_test_merge_free_blocks,
        // kfree_null_pointer_test,
        // kfree_double_free_test,
        // kfree_invalid_pointer_inside_heap_test,
        // kfree_invalid_pointer_outside_heap_test,
        kfree_merge_order_test,
    };

    const char *names[] = {
        "single_alloc",
        "single_alloc_and_free",
        "merge_free_blocks",
        "kfree_null_pointer",
        "kfree_double_free",
        "kfree_invalid_inside_heap",
        "kfree_invalid_outside_heap",
        "kfree_merge_order",
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int test_passed = 0;

    for (int i = 0; i < num_tests; i++)
    {
        printf("Running test %d (%s): ", i, names[i]);
        setup();
        int result = tests[i]();
        tear_down();

        if (!result)
        {
            printf("FAILED\n");
            return 1;
        }
        printf("PASSED\n");
        test_passed++;
    }
    printf("%d/%d tests passed\n", test_passed, num_tests);
    return 0;
}
