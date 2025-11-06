#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @enum block_state
     * @brief Enumeration of block states in the memory allocator.
     *
     * This enum defines the possible states of a memory block in the
     * kernel memory allocator.
     */
    enum block_state {
        BLOCK_FREE = 0, /**< Block is free and available for allocation. */
        BLOCK_USED      /**< Block is currently allocated and in use. */
    };

    /**
     * @brief Header structure for memory blocks in the allocator.
     *
     * Each allocated or free block in the kernel memory allocator is
     * preceded by a header that contains metadata about the block.
     */
    struct header {
        size_t           size;     /**< Size of the memory block (excluding header). */
        enum block_state state;    /**< State of the block (free or used). */
        struct header    *next;    /**< Pointer to the next block in the linked list. */
        struct header    *prev;    /**< Pointer to the previous block in the linked list. */
    };

    /**
    * @brief Retrieves the head of the kmalloc allocation list.
    *
    * This function returns a pointer to the head of the memory allocation
    * list used by kmalloc. Mainly useful for testing current allocations.
    *
    * @return Pointer to the head of the kmalloc allocation list.
    *         Returns NULL if the list is empty.
    */
    struct header *kmalloc_get_head(void);

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
    void   kmalloc_init(void *start, void *limit);

    /**
    * @brief Allocate a block of memory from the kernel heap.
    * 
    * This function searches the heap for the first free block that is large
    * enough to satisfy the requested `size`. If the block is larger than needed,
    * it is split into an allocated block and a new free block.
    *
    * @param size The number of bytes to allocate. Must be > 0.
    *
    * @return Pointer to the usable memory area of the allocated block, or
    *         NULL if `size` is zero.
    *
    * @warning If no suitable block is found, the function will call `kernel_panic`.
    * @note The returned pointer points immediately after the block header.
    *
    */
    void*  kmalloc(size_t size);

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
    void   kfree(void *block);
    int    kmalloc_test(void);
#ifdef __cplusplus
}
#endif

