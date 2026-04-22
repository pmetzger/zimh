// SPDX-FileCopyrightText: The ZIMH Project
// SPDX-License-Identifier: MIT

#ifndef H_SIM_COMPAT
#define H_SIM_COMPAT 1

/* `src/compat` compatibility routines.

   These declarations expose small libc-style interfaces that are not
   available on all supported host platforms. */

#include <stddef.h>

size_t strlcpy(char *dst, const char *src, size_t dsize);

size_t strlcat(char *dst, const char *src, size_t dsize);

#endif /* H_SIM_COMPAT */
