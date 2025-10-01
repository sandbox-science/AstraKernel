#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void  kmalloc_init(void *start, void *limit);
    void* kmalloc(size_t size);
    size_t kmalloc_remaining(void);

#ifdef __cplusplus
}
#endif

