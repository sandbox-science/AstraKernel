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

    struct header *kmalloc_get_head(void);

    void   kmalloc_init(void *start, void *limit);
    void*  kmalloc(size_t size);
    void   kfree(void *block);
    int    kmalloc_test(void);
#ifdef __cplusplus
}
#endif

