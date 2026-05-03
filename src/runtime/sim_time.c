/* sim_time.c: Time API wrappers for ZIMH */
// SPDX-FileCopyrightText: 2026 The ZIMH Project
// SPDX-License-Identifier: MIT

#include "sim_defs.h"
#include "sim_time.h"

/* Tests can replace these thin wrappers to drive clock and sleep paths
   deterministically without depending on the real host clock. */
static sim_clock_gettime_fn sim_clock_gettime_hook = NULL;
static sim_nanosleep_fn sim_nanosleep_hook = NULL;

/* Call the platform clock API used for wall-clock lookups. */
static int sim_time_clock_gettime_default(int clock_id, struct timespec *tp)
{
    return clock_gettime(clock_id, tp);
}

/* Call the platform sleep API used for interruptible delays. */
static int sim_time_nanosleep_default(const struct timespec *req,
                                      struct timespec *rem)
{
#if defined(_WIN32)
    DWORD msec =
        (DWORD)((req->tv_sec * 1000) + ((req->tv_nsec + 999999L) / 1000000L));

    Sleep(msec);
    if (rem != NULL) {
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
    }
    return 0;
#else
    return nanosleep(req, rem);
#endif
}

/* Fetch one timestamp through the active clock wrapper. */
int sim_clock_gettime(int clock_id, struct timespec *tp)
{
    sim_clock_gettime_fn clock_fn = sim_clock_gettime_hook
                                        ? sim_clock_gettime_hook
                                        : sim_time_clock_gettime_default;

    return clock_fn(clock_id, tp);
}

/* Sleep through the active nanosleep wrapper. */
int sim_nanosleep(const struct timespec *req, struct timespec *rem)
{
    sim_nanosleep_fn sleep_fn =
        sim_nanosleep_hook ? sim_nanosleep_hook : sim_time_nanosleep_default;

    return sleep_fn(req, rem);
}

/* Sleep for an interval, retrying after EINTR with the remaining time. */
static void sim_sleep_timespec(struct timespec req)
{
    struct timespec rem = {0};

    while (sim_nanosleep(&req, &rem) != 0) {
        if (errno != EINTR)
            return;
        req = rem;
    }
}

/* Return the current wall-clock seconds value in time(3) form. */
time_t sim_time(time_t *now)
{
    struct timespec ts_now;

    if (sim_clock_gettime(CLOCK_REALTIME, &ts_now) != 0) {
        if (now != NULL)
            *now = (time_t)-1;
        return (time_t)-1;
    }
    if (now != NULL)
        *now = ts_now.tv_sec;
    return ts_now.tv_sec;
}

/* Sleep for a whole-second interval, retrying after EINTR. */
void sim_sleep(unsigned int sec)
{
    struct timespec req = {.tv_sec = (time_t)sec, .tv_nsec = 0};

    sim_sleep_timespec(req);
}

/* Sleep for a millisecond interval through sim_nanosleep(). */
void sim_sleep_msec(unsigned int msec)
{
    struct timespec req = {
        .tv_sec = (time_t)(msec / 1000),
        .tv_nsec = (long)((msec % 1000) * 1000000UL),
    };

    sim_sleep_timespec(req);
}

/* Install deterministic test hooks for clock and sleep calls. */
void sim_time_set_test_hooks(sim_clock_gettime_fn clock_hook,
                             sim_nanosleep_fn sleep_hook)
{
    sim_clock_gettime_hook = clock_hook;
    sim_nanosleep_hook = sleep_hook;
}

/* Restore the default time hooks after a test case completes. */
void sim_time_reset_test_hooks(void)
{
    sim_time_set_test_hooks(NULL, NULL);
}
