/* sim_time_win32.h: Windows compatibility for ZIMH time wrappers */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#ifndef SIM_TIME_WIN32_H_
#define SIM_TIME_WIN32_H_ 0

#if !defined(CLOCK_REALTIME)
#define CLOCK_REALTIME 1
#endif

#if !defined(HAVE_STRUCT_TIMESPEC)
#define HAVE_STRUCT_TIMESPEC
#if !defined(_TIMESPEC_DEFINED)
#define _TIMESPEC_DEFINED
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
#endif
#endif

#if !defined(NEED_CLOCK_GETTIME_DECL)
#define NEED_CLOCK_GETTIME_DECL 1
int clock_gettime(int clock_id, struct timespec *tp);
#endif

#endif
