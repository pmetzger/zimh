// SPDX-FileCopyrightText: The ZIMH Project
// SPDX-License-Identifier: MIT

#ifndef H_SIM_STRING_COMPAT
#define H_SIM_STRING_COMPAT 1

/*
 * String compatibility routines that may be missing on several hosts.
 * Do not override fortified libc macros here; tests that need to call the
 * shim directly must undefine those macros locally.
 */

#include <stddef.h>

#if defined(SIMH_NEED_STRLCPY) && !defined(strlcpy)
size_t strlcpy(char *dst, const char *src, size_t dsize);
#endif

#if defined(SIMH_NEED_STRLCAT) && !defined(strlcat)
size_t strlcat(char *dst, const char *src, size_t dsize);
#endif

#endif /* H_SIM_STRING_COMPAT */
