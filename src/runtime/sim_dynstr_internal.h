/* sim_dynstr_internal.h: Internal dynamic string test hooks */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#ifndef SIM_DYNSTR_INTERNAL_H_
#define SIM_DYNSTR_INTERNAL_H_ 0

#include <stdarg.h>

#include "sim_defs.h"

/* Test hook for the reallocator used by dynamic strings. */
typedef void *(*sim_dynstr_realloc_fn)(void *ptr, size_t size);

/* Test hook for formatted output sizing and rendering. */
typedef int (*sim_dynstr_vsnprintf_fn)(char *buf, size_t size, const char *fmt,
                                       va_list args) PRINTF_FMT(3, 0);

/* Install a test reallocator hook for deterministic failure coverage. */
void sim_dynstr_set_test_realloc_hook(sim_dynstr_realloc_fn hook);

/* Install a test formatting hook for deterministic error coverage. */
void sim_dynstr_set_test_vsnprintf_hook(sim_dynstr_vsnprintf_fn hook);

/* Restore the default realloc-backed allocator hook. */
void sim_dynstr_reset_test_hooks(void);

#endif
