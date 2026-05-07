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
- the stale Windows-side `NEED_CLOCK_GETTIME` shim is gone; Windows
  realtime clock handling now lives in `sim_time.c`
- `sim_timer.c` has substantial deterministic unit coverage for the
  non-async timer paths, including:
  - host time wrapper seams and deadline construction
  - timer initialization and calibration
  - catch-up tick bookkeeping
  - activation and long-delay coscheduling
  - negative wall-clock activation delay rejection
  - idle behavior and throttle state-machine behavior
  - ROM delay word handling
- the `sim_timer_cancel()` return type now matches its status-style
  return values
- `sim_cancel()` now propagates `sim_timer_cancel()` failures for
  `UNIT_TMR_UNIT` clock units instead of silently treating stale timer
  markers as normal inactive units

What remains:

- decide how to handle `pthread_cond_timedwait()` under the current time
  policy
  - this is the main gap in the present migration
  - likely options are:
    - leave the timed-wait clock acquisition as an explicit exception
    - keep the current shared deadline-building helper but treat the
      `pthread_cond_timedwait()` call itself as the remaining exception
    - or otherwise isolate these absolute-time wait paths more cleanly
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
- the suite is now meaningful for the synchronous timer code rather
  than just a foothold
- a recent focused coverage run after the non-async expansion was about:
  - functions: 88%
  - lines: 81%
  - branches: 51%
- this is enough to support focused changes in the covered paths, but
  it should not be treated as full coverage of the module
- the largest remaining coverage gap is the async-clock/timer-thread
  side, where practical tests probably require additional seams or
  helper extraction

Testing work that remains:

- deepen unit coverage into meaningful timer behavior, especially:
  - the status/reporting paths recently converted away from `ctime()`
  - additional calibration and catch-up edge cases where deterministic
    coverage is feasible
  - async-clock and timer-thread logic where narrow seams or helper
    extraction can make tests practical
- keep adding direct coverage before changing uncovered timer behavior

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
