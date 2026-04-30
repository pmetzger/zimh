/* setenv.c: Windows compatibility for setenv and unsetenv */
// SPDX-FileCopyrightText: The ZIMH Project
// SPDX-License-Identifier: MIT

#include "sim_win32_compat.h"

#if defined(_WIN32)

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* Install or update one environment variable using the Windows CRT. */
int setenv(const char *envname, const char *envval, int overwrite)
{
    int status;

    if ((envname == NULL) || (envval == NULL) ||
        (strchr(envname, '=') != NULL) || (*envname == '\0')) {
        errno = EINVAL;
        return -1;
    }
    if (!overwrite && (getenv(envname) != NULL))
        return 0;
    status = _putenv_s(envname, envval);
    if (status != 0) {
        errno = status;
        return -1;
    }
    return 0;
}

/* Remove one environment variable using the Windows CRT. */
int unsetenv(const char *envname)
{
    if ((envname == NULL) || (strchr(envname, '=') != NULL) ||
        (*envname == '\0')) {
        errno = EINVAL;
        return -1;
    }
    return setenv(envname, "", 1);
}

#endif
