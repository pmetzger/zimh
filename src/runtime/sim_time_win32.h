/* sim_time_win32.h: Windows compatibility for ZIMH time wrappers */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#ifndef SIM_TIME_WIN32_H_
#define SIM_TIME_WIN32_H_ 0

#if !defined(CLOCK_REALTIME)
#define CLOCK_REALTIME 1
#endif

int clock_gettime(int clock_id, struct timespec *tp);

#endif
