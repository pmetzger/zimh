/* sim_dynstr.c: Dynamic string support for ZIMH */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#include "sim_dynstr.h"

/* Tests can replace the allocator to drive allocation-failure paths. */
static sim_dynstr_realloc_fn sim_dynstr_realloc_hook = NULL;

/* Use the host allocator for normal dynamic-string growth. */
static void *sim_dynstr_default_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

/* Resolve the realloc implementation currently in force. */
static sim_dynstr_realloc_fn sim_dynstr_realloc_impl(void)
{
    return sim_dynstr_realloc_hook ? sim_dynstr_realloc_hook
                                   : sim_dynstr_default_realloc;
}

/* Ensure the string buffer can hold the requested byte count. */
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

/* Initialize one dynamic string to the empty state. */
void sim_dynstr_init(sim_dynstr_t *ds)
{
    ds->buf = NULL;
    ds->len = 0;
    ds->cap = 0;
}

/* Release any storage owned by one dynamic string. */
void sim_dynstr_free(sim_dynstr_t *ds)
{
    free(ds->buf);
    sim_dynstr_init(ds);
}

/* Append one NUL-terminated string to the current contents. */
t_bool sim_dynstr_append(sim_dynstr_t *ds, const char *text)
{
    size_t text_len = strlen(text);

    if (!sim_dynstr_reserve(ds, ds->len + text_len + 1))
        return FALSE;
    memcpy(ds->buf + ds->len, text, text_len + 1);
    ds->len += text_len;
    return TRUE;
}

/* Append one character and keep the buffer NUL-terminated. */
t_bool sim_dynstr_append_ch(sim_dynstr_t *ds, char ch)
{
    if (!sim_dynstr_reserve(ds, ds->len + 2))
        return FALSE;
    ds->buf[ds->len++] = ch;
    ds->buf[ds->len] = '\0';
    return TRUE;
}

/* Return the current contents as a conventional C string view. */
const char *sim_dynstr_cstr(const sim_dynstr_t *ds)
{
    return ds->buf ? ds->buf : "";
}

/* Install a test allocator hook for deterministic failure coverage. */
void sim_dynstr_set_test_realloc_hook(sim_dynstr_realloc_fn hook)
{
    sim_dynstr_realloc_hook = hook;
}

/* Restore the default allocator after one test case completes. */
void sim_dynstr_reset_test_hooks(void)
{
    sim_dynstr_set_test_realloc_hook(NULL);
}
