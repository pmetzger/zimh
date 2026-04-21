/* sim_time.h: thin time wrappers for deterministic tests

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

#ifndef SIM_TIME_H_
#define SIM_TIME_H_ 0

#include <time.h>

#if defined(_WIN32)
#include "sim_time_win32.h"
#endif

/* Test hook for the clock source used by sim_clock_gettime(). */
typedef int (*sim_clock_gettime_fn)(int clock_id, struct timespec *tp);

/* Test hook for the sleep primitive used by sim_nanosleep(). */
typedef int (*sim_nanosleep_fn)(const struct timespec *req,
                                struct timespec *rem);

/* Thin wrapper around clock_gettime(). */
int sim_clock_gettime(int clock_id, struct timespec *tp);

/* Thin wrapper around nanosleep(). */
int sim_nanosleep(const struct timespec *req, struct timespec *rem);

/* Return the current CLOCK_REALTIME seconds value. */
time_t sim_time(time_t *now);

/* Sleep for a whole number of seconds through sim_nanosleep(). */
void sim_sleep(unsigned int sec);

/* Install test hooks for deterministic clock and sleep behavior. */
void sim_time_set_test_hooks(sim_clock_gettime_fn clock_hook,
                             sim_nanosleep_fn sleep_hook);

/* Restore the default host-backed clock and sleep hooks. */
void sim_time_reset_test_hooks(void);

#endif
