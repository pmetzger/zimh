/* sim_dynstr.h: Dynamic string support for ZIMH */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#ifndef SIM_DYNSTR_H_
#define SIM_DYNSTR_H_ 0

#include <stdarg.h>

#include "sim_defs.h"

typedef struct sim_dynstr {
    char *buf;
    size_t len;
    size_t cap;
} sim_dynstr_t;

/* Test hook for the reallocator used by dynamic strings. */
typedef void *(*sim_dynstr_realloc_fn)(void *ptr, size_t size);

/* Test hook for formatted output sizing and rendering. */
typedef int (*sim_dynstr_vsnprintf_fn)(char *buf, size_t size, const char *fmt,
                                       va_list args) PRINTF_FMT(3, 0);

/* Initialize one empty dynamic string. */
void sim_dynstr_init(sim_dynstr_t *ds);

/* Release storage owned by one dynamic string and reset it. */
void sim_dynstr_free(sim_dynstr_t *ds);

/* Append one NUL-terminated string. */
t_bool sim_dynstr_append(sim_dynstr_t *ds, const char *text);

/* Append one formatted string fragment. */
t_bool sim_dynstr_appendf(sim_dynstr_t *ds, const char *fmt,
                          ...) PRINTF_FMT(2, 3);

/* Append one single character. */
t_bool sim_dynstr_append_ch(sim_dynstr_t *ds, char ch);

/* Return the current contents as a conventional C string. */
const char *sim_dynstr_cstr(const sim_dynstr_t *ds);

/* Remove any leading characters that appear in the trim set. */
void sim_dynstr_ltrim_chars(sim_dynstr_t *ds, const char *trimchars);

/* Transfer ownership of the backing buffer and reset the string. */
char *sim_dynstr_take(sim_dynstr_t *ds);

/* Install a test reallocator hook for deterministic failure coverage. */
void sim_dynstr_set_test_realloc_hook(sim_dynstr_realloc_fn hook);

/* Install a test formatting hook for deterministic error coverage. */
void sim_dynstr_set_test_vsnprintf_hook(sim_dynstr_vsnprintf_fn hook);

/* Restore the default realloc-backed allocator hook. */
void sim_dynstr_reset_test_hooks(void);

#endif
