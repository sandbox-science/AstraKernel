#include "errno.h"

const char* error_str(kerror_t error_code)
{
    switch (error_code)
    {
        case KERR_OK:
            return "ok";
        case KERR_NOT_FOUND:
            return "not found";
        case KERR_NOMEM:
            return "out of memory";
        case KERR_NO_SPACE:
            return "out of space";
        case KERR_INVAL:
            return "invalid request";
        default:
            return "unknown error";
    }
}

