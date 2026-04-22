/* gmtime_r.c: Windows compatibility for gmtime_r */
// SPDX-FileCopyrightText: The ZIMH Project
// SPDX-License-Identifier: MIT

#include "sim_compat.h"

struct tm *gmtime_r(const time_t *timer, struct tm *result)
{
    if (result == NULL || timer == NULL)
        return NULL;
    return (gmtime_s(result, timer) == 0) ? result : NULL;
}
