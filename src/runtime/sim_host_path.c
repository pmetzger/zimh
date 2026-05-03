/* sim_host_path.c: Host path-name discovery helpers

   This module owns reusable host path-name answers that are independent
   of a specific file operation. Keep platform lookup order and API
   selection here when callers need a host path spelling rather than
   direct file I/O.
*/
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#include "sim_host_path_internal.h"

#include <stdlib.h>

#include "sim_defs.h"

#if defined(_WIN32)
static sim_host_win32_temp_path_fn sim_win32_get_temp_path2_hook = NULL;
static sim_host_win32_temp_path_fn sim_win32_get_temp_path_hook = NULL;
static t_bool sim_win32_temp_path_hooks_installed = FALSE;

/* Return the Windows GetTempPath2A result, or zero if it is unavailable. */
static DWORD sim_win32_default_get_temp_path2(DWORD buf_size, LPSTR buf)
{
    typedef DWORD(WINAPI * get_temp_path2_fn)(DWORD, LPSTR);
    HMODULE kernel32;
    get_temp_path2_fn get_temp_path2 = NULL;

    kernel32 = GetModuleHandleA("kernel32.dll");
    if (kernel32 != NULL)
        get_temp_path2 =
            (get_temp_path2_fn)GetProcAddress(kernel32, "GetTempPath2A");
    return get_temp_path2 == NULL ? 0 : get_temp_path2(buf_size, buf);
}

/* Return the Windows GetTempPathA result. */
static DWORD sim_win32_default_get_temp_path(DWORD buf_size, LPSTR buf)
{
    return GetTempPathA(buf_size, buf);
}

/* Install Windows temporary-directory API test hooks. */
void sim_host_path_set_test_win32_temp_hooks(
    sim_host_win32_temp_path_fn get_temp_path2,
    sim_host_win32_temp_path_fn get_temp_path)
{
    sim_win32_get_temp_path2_hook = get_temp_path2;
    sim_win32_get_temp_path_hook = get_temp_path;
    sim_win32_temp_path_hooks_installed = TRUE;
}

/* Restore default host-backed path hooks. */
void sim_host_path_reset_test_hooks(void)
{
    sim_win32_get_temp_path2_hook = NULL;
    sim_win32_get_temp_path_hook = NULL;
    sim_win32_temp_path_hooks_installed = FALSE;
}

/* Return the Windows temporary-directory path from the best available API. */
static t_bool sim_win32_temp_dir(char *buf, size_t buf_size)
{
    sim_host_win32_temp_path_fn get_temp_path2;
    sim_host_win32_temp_path_fn get_temp_path;
    DWORD length;

    if (sim_win32_temp_path_hooks_installed) {
        get_temp_path2 = sim_win32_get_temp_path2_hook;
        get_temp_path = sim_win32_get_temp_path_hook;
    } else {
        get_temp_path2 =
            (sim_host_win32_temp_path_fn)sim_win32_default_get_temp_path2;
        get_temp_path =
            (sim_host_win32_temp_path_fn)sim_win32_default_get_temp_path;
    }

    length = get_temp_path2 == NULL ? 0 : get_temp_path2((DWORD)buf_size, buf);
    if (length > 0)
        return (size_t)length < buf_size;

    if (length == 0)
        length =
            get_temp_path == NULL ? 0 : get_temp_path((DWORD)buf_size, buf);
    return (length > 0) && ((size_t)length < buf_size);
}
#endif

/* Return the host temporary-file directory path in caller-provided storage. */
const char *sim_host_temp_dir(char *buf, size_t buf_size)
{
    static const char *const env_names[] = {
#if defined(_WIN32)
        "TMP",
        "TEMP",
        "USERPROFILE",
#else
        "TMPDIR",
        "TMP",
        "TEMP",
#endif
        NULL,
    };
    size_t i;

    if ((buf == NULL) || (buf_size == 0))
        return NULL;

#if defined(_WIN32)
    if (sim_win32_temp_dir(buf, buf_size))
        return buf;
#endif

    for (i = 0; env_names[i] != NULL; ++i) {
        const char *value = getenv(env_names[i]);

        if ((value != NULL) && (value[0] != '\0')) {
            strlcpy(buf, value, buf_size);
            return buf;
        }
    }
#if defined(_WIN32)
    strlcpy(buf, ".", buf_size);
#else
    /* Use the conventional command-file fallback, not C's P_tmpdir. */
    strlcpy(buf, "/tmp", buf_size);
#endif
    return buf;
}
