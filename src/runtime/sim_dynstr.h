/* sim_dynstr.h: Dynamic string support for ZIMH

   Copyright (c) 2026, The ZIMH Project

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   THE ZIMH PROJECT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SIM_DYNSTR_H_
#define SIM_DYNSTR_H_ 0

#include "sim_defs.h"

typedef struct sim_dynstr {
    char *buf;
    size_t len;
    size_t cap;
} sim_dynstr_t;

/* Test hook for the reallocator used by dynamic strings. */
typedef void *(*sim_dynstr_realloc_fn)(void *ptr, size_t size);

/* Initialize one empty dynamic string. */
void sim_dynstr_init(sim_dynstr_t *ds);

/* Release storage owned by one dynamic string and reset it. */
void sim_dynstr_free(sim_dynstr_t *ds);

/* Append one NUL-terminated string. */
t_bool sim_dynstr_append(sim_dynstr_t *ds, const char *text);

/* Append one single character. */
t_bool sim_dynstr_append_ch(sim_dynstr_t *ds, char ch);

/* Return the current contents as a conventional C string. */
const char *sim_dynstr_cstr(const sim_dynstr_t *ds);

/* Install a test reallocator hook for deterministic failure coverage. */
void sim_dynstr_set_test_realloc_hook(sim_dynstr_realloc_fn hook);

/* Restore the default realloc-backed allocator hook. */
void sim_dynstr_reset_test_hooks(void);

#endif
