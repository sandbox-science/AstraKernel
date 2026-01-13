#include <stddef.h>
#include <stdint.h>
#include "datetime.h"

_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");

// TODO: Potentially move those macros to utils file or something
#define     RTCDR (*(volatile uint32_t *)0x101e8000) // RTC Register

#define     SECONDS_IN_YEAR         31556926
#define     SECONDS_IN_MONTH        2629743
#define     SECONDS_IN_DAY          86400
#define     LEAP_YEARS_BEFORE_1970  477

uint32_t getdate(dateval *date_struct)
{
    static uint32_t day_arr[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // Initialize only once

    if (date_struct != NULL)
    {
        uint32_t since_epoch = RTCDR;

        date_struct->year  = 1970 + since_epoch / SECONDS_IN_YEAR;
        date_struct->month = 1 + (since_epoch / SECONDS_IN_MONTH) % 12;

        uint32_t days_since_epoch       = since_epoch / SECONDS_IN_DAY;
        uint32_t leap_years_since_epoch = (date_struct->year / 4) - (date_struct->year / 100) + (date_struct->year / 400) - LEAP_YEARS_BEFORE_1970;
        uint32_t not_leap_years         = date_struct->year - 1970 - leap_years_since_epoch;
        
        date_struct->day = days_since_epoch - (leap_years_since_epoch * 366) - (not_leap_years * 365);

        uint32_t is_leap_year = ((date_struct->year % 4 == 0) && (date_struct->year % 100 != 0)) || (date_struct->year % 400 == 0);
        day_arr[1] += is_leap_year; // Extra day if leap year

        for (uint32_t i = 0; i < 12; i++)
        {
            if (date_struct->day > day_arr[i])
            {
                date_struct->day -= day_arr[i];
            }
        }

        day_arr[1] -= is_leap_year; // Undo the leap year effect, if any
    }

    return RTCDR;
}

uint32_t gettime(timeval *time_struct)
{
    if (time_struct != NULL)
    {
        uint32_t since_epoch = RTCDR;

        time_struct->secs = since_epoch % 60;
        since_epoch /= 60;

        time_struct->mins = since_epoch % 60;
        since_epoch /= 60;

        time_struct->hrs = since_epoch % 24;
    }

    return RTCDR;
}
