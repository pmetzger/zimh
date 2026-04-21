/* sim_dynstr.c: Dynamic string support for ZIMH

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

#include "sim_dynstr.h"

/* Tests can replace the allocator to drive allocation-failure paths. */
static sim_dynstr_realloc_fn sim_dynstr_realloc_hook = NULL;

static void *sim_dynstr_default_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

static sim_dynstr_realloc_fn sim_dynstr_realloc_impl(void)
{
    return sim_dynstr_realloc_hook ? sim_dynstr_realloc_hook
                                   : sim_dynstr_default_realloc;
}

static t_bool sim_dynstr_reserve(sim_dynstr_t *ds, size_t needed)
{
    sim_dynstr_realloc_fn realloc_fn;
    size_t new_cap;
    char *new_buf;

    if (needed <= ds->cap)
        return TRUE;
    new_cap = ds->cap ? ds->cap : 16;
    while (new_cap < needed)
        new_cap *= 2;
    realloc_fn = sim_dynstr_realloc_impl();
    new_buf = (char *)realloc_fn(ds->buf, new_cap);
    if (new_buf == NULL)
        return FALSE;
    ds->buf = new_buf;
    ds->cap = new_cap;
    return TRUE;
}

void sim_dynstr_init(sim_dynstr_t *ds)
{
    ds->buf = NULL;
    ds->len = 0;
    ds->cap = 0;
}

void sim_dynstr_free(sim_dynstr_t *ds)
{
    free(ds->buf);
    sim_dynstr_init(ds);
}

t_bool sim_dynstr_append(sim_dynstr_t *ds, const char *text)
{
    size_t text_len = strlen(text);

    if (!sim_dynstr_reserve(ds, ds->len + text_len + 1))
        return FALSE;
    memcpy(ds->buf + ds->len, text, text_len + 1);
    ds->len += text_len;
    return TRUE;
}

t_bool sim_dynstr_append_ch(sim_dynstr_t *ds, char ch)
{
    if (!sim_dynstr_reserve(ds, ds->len + 2))
        return FALSE;
    ds->buf[ds->len++] = ch;
    ds->buf[ds->len] = '\0';
    return TRUE;
}

const char *sim_dynstr_cstr(const sim_dynstr_t *ds)
{
    return ds->buf ? ds->buf : "";
}

void sim_dynstr_set_test_realloc_hook(sim_dynstr_realloc_fn hook)
{
    sim_dynstr_realloc_hook = hook;
}

void sim_dynstr_reset_test_hooks(void)
{
    sim_dynstr_set_test_realloc_hook(NULL);
}
