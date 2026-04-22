/* scp_cmdvars.h: SCP command-variable substitution helpers

   Copyright (c) 1993-2022, Robert M Supnik

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
   ROBERT M SUPNIK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef SCP_CMDVARS_H_
#define SCP_CMDVARS_H_ 0

#include "sim_defs.h"
#if !defined(_WIN32)
#include <sys/utsname.h>
#endif

/* Implement SET ENVIRONMENT, including prompt-driven and decoded-string
   forms. */
t_stat sim_set_environment(int32 flag, CONST char *cptr);

/* Expand one command line in place using SCP command-variable rules. */
void sim_sub_args(char *in_str, size_t in_str_size, char *do_arg[]);

/* Resolve one SCP substitution name from built-ins, internal variables,
   saved external aliases, or the host environment. */
const char *_sim_get_env_special(const char *gbuf, char *rbuf,
                                 size_t rbuf_size);

/* Set or replace one SCP-owned substitution variable. */
void sim_sub_var_set(const char *name, const char *value);

/* Remove one SCP-owned substitution variable, if present. */
void sim_sub_var_unset(const char *name);

/* Remove every SCP-owned substitution variable with one common prefix. */
void sim_sub_var_clear_prefix(const char *prefix);

/* Map a substituted pointer back to its original command text. */
const char *sim_unsub_args(const char *cptr);

/* Capture one external host variable whose name collides with an SCP alias. */
void sim_cmdvars_capture_env_alias(const char *name);

/* Execute one host command while restoring saved external env aliases. */
int sim_cmdvars_system(const char *command);

#if !defined(_WIN32)
typedef int (*sim_cmdvars_uname_hook_fn)(struct utsname *utsname_info);

/* Replace the underlying uname(3) call used by SIM_OSTYPE lookup. */
void sim_cmdvars_set_test_uname_hook(sim_cmdvars_uname_hook_fn hook);
#endif
typedef t_bool (*sim_cmdvars_localtime_hook_fn)(time_t now, struct tm *tmnow);

/* Replace the underlying localtime call used by time-variable lookup. */
void sim_cmdvars_set_test_localtime_hook(sim_cmdvars_localtime_hook_fn hook);

/* Clear the cached host OS type used by SIM_OSTYPE lookup. */
void sim_cmdvars_reset_ostype_cache(void);

/* Reset command-variable state owned by this module. */
void sim_cmdvars_reset(void);

#endif
