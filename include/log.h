/**
 * @file log.h
 * @brief Minimal kernel logging helpers.
 *
 * Provides a tiny logging macro that prefixes messages with a level string.
 * Optionally includes a tick count when KLOG_USE_TICKS is defined.
 */
#pragma once

#include <stdint.h>

#include "printf.h"

#ifdef KLOG_USE_TICKS
#include "interrupt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum klog_level : uint8_t
{
    KLOG_DEBUG = 0,
    KLOG_INFO,
    KLOG_WARN,
    KLOG_ERROR,
    KLOG_PANIC
} klog_level_t;

static inline const char *klog_level_str(klog_level_t level)
{
    switch (level)
    {
        case KLOG_DEBUG: return "DEBUG";
        case KLOG_INFO:  return "INFO";
        case KLOG_WARN:  return "WARN";
        case KLOG_ERROR: return "ERROR";
        case KLOG_PANIC: return "PANIC";
        default:         return "LOG";
    }
}

#ifdef KLOG_USE_TICKS
#define KLOG(level, fmt, ...) \
    printf("[%s %lu] " fmt "\r\n", klog_level_str(level), \
           (unsigned long)systicks, ##__VA_ARGS__)
#else
#define KLOG(level, fmt, ...) \
    printf("[%s] " fmt "\r\n", klog_level_str(level), ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif
