#include <stddef.h>
#include <stdint.h>
#include "datetime.h"

_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");

#define RTCDR (*(volatile uint32_t *)0x101e8000) // RTC Register

uint32_t getdate(dateval *date_struct)
{
    static uint32_t day_arr[12] = {31, 27, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (date_struct != NULL)
    {
        uint32_t since_epoch = RTCDR;

        date_struct->year = 1970 + since_epoch / 31556926;

        date_struct->month = 1 + (since_epoch / 2629743) % 12;

        uint32_t days_since_epoch = since_epoch / 86400;
        // 477 is number of leap years before 1970
        uint32_t leap_years_since_epoch = (date_struct->year / 4) - (date_struct->year / 100) + (date_struct->year / 400) - 477;

        uint32_t is_leap_year = ((date_struct->year % 4 == 0) && (date_struct->year % 100 != 0)) || (date_struct->year % 400 == 0);

        uint32_t not_leap_years = date_struct->year - 1970 - leap_years_since_epoch;

        date_struct->day = days_since_epoch - leap_years_since_epoch * 366 - not_leap_years * 365;

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