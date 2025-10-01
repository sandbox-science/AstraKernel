#ifndef DATETIME_H
#define DATETIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint32_t hrs;
        uint32_t mins;
        uint32_t secs;
    } timeval;

    typedef struct
    {
        uint32_t day;
        uint32_t month;
        uint32_t year;

    } dateval;

    uint32_t getdate(dateval *date);
    uint32_t gettime(timeval *time_struct);

#ifdef __cplusplus
}
#endif

#endif // DATETIME_H
