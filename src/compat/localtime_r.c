/* localtime_r.c: Windows compatibility for localtime_r */
// SPDX-FileCopyrightText: The ZIMH Project
// SPDX-License-Identifier: MIT

#include "sim_compat.h"

struct tm *localtime_r(const time_t *timer, struct tm *result)
{
    if (result == NULL || timer == NULL)
        return NULL;
    return (localtime_s(result, timer) == 0) ? result : NULL;
}
