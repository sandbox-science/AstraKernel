#include "memory.h"
#include "printf.h"
#include "log.h"


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
        KLOG(KLOG_ERROR, "kmalloc returned NULL");
        return 0;
    }
    struct header *head = kmalloc_get_head();
    if (head->state != BLOCK_USED)
    {
        KLOG(KLOG_ERROR, "Block not marked as used after kmalloc");
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
        KLOG(KLOG_ERROR, "Block not free after kfree");
        return 0;
    }
    if (head->size != initial_heap_size)
    {
        KLOG(KLOG_ERROR, "Block size incorrect after kfree: got %lu expected %lu\n", head->size, initial_heap_size);
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
        KLOG(KLOG_ERROR, "Head not free after merging");
        return 0;
    }
    if (head->size != initial_heap_size)
    {
        KLOG(KLOG_ERROR, "Merged size incorrect: got %lu expected %lu\n", head->size, initial_heap_size);
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
        KLOG(KLOG_ERROR, "Head not free after out-of-order merging");
        return 0;
    }
    if (head->size != initial_heap_size)
    {
        KLOG(KLOG_ERROR, "Heap size incorrect after out-of-order merge: got %lu expected %lu\n", head->size, initial_heap_size);
        return 0;
    }
    return 1;
}

// --- Main test runner ---
int kmalloc_test()
{
    KLOG(KLOG_INFO, "Running kmalloc tests...");

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
        // "kfree_null_pointer",
        // "kfree_double_free",
        // "kfree_invalid_inside_heap",
        // "kfree_invalid_outside_heap",
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
            KLOG(KLOG_ERROR, "FAILED");
            return 1;
        }
        KLOG(KLOG_INFO, "PASSED");
        test_passed++;
    }
    KLOG(KLOG_INFO, "\nkmalloc_test() -> %d/%d tests passed!\n\n", test_passed, num_tests);
    return 0;
}