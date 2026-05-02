# SIM Timer Fixes

Goal: finish the policy, coverage, and structural cleanup work in
`src/runtime/sim_timer.c` so it conforms to the time-wrapper direction
in `TIME_TESTING.md` and becomes substantially easier to test.

What is already done:

- the obsolete Unix-side `clock_gettime()` fallback guarded by
  `!defined(_POSIX_SOURCE)` / `NEED_CLOCK_GETTIME` is gone
- the easy kernel-facing clock/sleep calls in `sim_timer.c` now use the
  shared `sim_time.[ch]` wrappers:
  - `sim_os_msec()`
  - `sim_os_sleep()`
  - `sim_os_ms_sleep()`
- the `ctime()` formatting hacks in the timer/status display paths were
  replaced with explicit broken-down-time formatting
- `sim_timer.c` now uses `localtime_r()` through the new compat layer
  instead of carrying an internal `_WIN32` split for that helper
- the remaining timed-wait code now builds deadlines from
  `sim_clock_gettime()` through a shared helper instead of reading the
  clock directly at each call site

What remains:

- decide how to handle `pthread_cond_timedwait()` under the current time
  policy
  - this is the main gap in the present migration
  - likely options are:
    - leave the timed-wait clock acquisition as an explicit exception
    - keep the current shared deadline-building helper but treat the
      `pthread_cond_timedwait()` call itself as the remaining exception
    - or otherwise isolate these absolute-time wait paths more cleanly
- remove or shrink the Windows-side `NEED_CLOCK_GETTIME` shim
  - once the file has been moved decisively onto `sim_clock_gettime()`,
    this local compatibility layer should be reassessed
- simplify the `_WIN32 || HAVE_WINMM` split if the extra `HAVE_WINMM`
  condition is no longer buying anything meaningful
- review the `NEED_THREAD_PRIORITY` / `#undef NEED_THREAD_PRIORITY`
  macro choreography
  - not a direct time-policy issue, but part of the file's outdated
    portability structure
  - likely worth simplifying while the timer module is being cleaned
- keep the distinction clear between:
  - host wall-clock / sleep policy
  - simulator-time APIs like `sim_gtime()` and timer scheduling
  These are not the same migration and should not be conflated

Coverage status:

- there is a dedicated unit-test target:
  - `zimh-unit-sim-timer`
- but it is still only a foothold suite
- current coverage from that test alone is extremely low:
  - functions: `6.33%`
  - lines: `1.68%`
  - branches: `0.66%`
- the current tests mostly cover:
  - `sim_timespec_diff()`
  - a few default-state helper/query behaviors
  - the new absolute-deadline helper
  - `sim_os_msec()` and `sim_os_ms_sleep()` through the shared
    `sim_time` seam
- they do not yet cover the real scheduling, calibration, async-clock,
  or reporting logic

Testing work that remains:

- deepen unit coverage into meaningful timer behavior, especially:
  - `sim_os_msec()` / `sim_os_ms_sleep()` behavior through the shared
    `sim_time` seam
  - the status/reporting paths recently converted away from `ctime()`
  - calibration and catch-up logic where deterministic coverage is
    feasible
  - async-clock and timer-thread logic where narrow seams or helper
    extraction can make tests practical
- do not rely on the current foothold tests as evidence that
  `sim_timer.c` is well covered; it is not

Structural cleanup ideas:

- separate backend/platform plumbing from timer policy more clearly
- separate status/reporting formatting from timer arithmetic and
  scheduling logic
- isolate the kernel-facing time/sleep boundary from the rest of the
  module so coverage and review can focus on timer policy rather than
  system-call noise
- consider whether some timer reporting helpers should be extracted into
  pure formatting helpers, as was done successfully in other modules

Rule:

- If new architectural cleanup opportunities or time-policy gaps are
  discovered while working on `sim_timer.c`, add them here instead of
  silently deferring them.
