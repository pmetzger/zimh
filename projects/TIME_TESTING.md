# Time Testing

## Goal

Introduce a small, centralized time seam so tests can drive
time-dependent behavior deterministically.

This is not meant to become a large simulated-clock framework. The
initial goal is only to make date, time, timeout, and elapsed-time code
testable without depending on the real host clock.

## Chosen Direction

The right split is:

- wrap only the kernel-facing calls that obtain or wait on time
- standardize direct use of the POSIX calls that only process timestamps

In practice that means:

- wrap:
  - `clock_gettime()`
  - `nanosleep()`
- standardize direct use of:
  - `localtime_r()`
  - `gmtime_r()`
  - `strftime()`
  - `mktime()`
  - `difftime()`

And replace direct calls to:

- `time()`
- `localtime()`
- `gmtime()`
- `ctime()`
- `asctime()`
- `gettimeofday()`
- `sleep()`
- `usleep()`

The wrappers should be thin, obvious interposition points, not a broad
new time API.

## Wrapper Model

The wrapper layer should live in:

- `src/runtime/sim_time.[ch]`

The initial wrapper set should likely be:

- `sim_clock_gettime()`
- `sim_nanosleep()`
- `sim_time()`
- `sim_sleep()`

`sim_time()` should be a convenience wrapper for the common
"current wall-clock seconds" case by calling `sim_clock_gettime()` and
returning `tv_sec`.

`sim_sleep()` should be a convenience wrapper over `sim_nanosleep()`
for code that currently sleeps in whole seconds.

Production code should call the real system functions through these thin
wrappers. Tests can override the wrappers.

Everything else should be rewritten to consume the resulting `time_t`,
`struct timespec`, or `struct tm` directly with normal POSIX helpers.

## Audit Results

The current tree uses a mix of time APIs.

### SCP Command Variables

`src/core/scp_cmdvars.c` currently uses:

- `time()`
- `localtime()`
- `ctime()`
- `strftime()`

This should be converted to:

- `sim_time()` or `sim_clock_gettime()` to obtain the current wall-clock
  time
- `localtime_r()` to break it down
- `strftime()` for most formatting
- direct formatting from `struct tm` / `time_t` where `ctime()` is now
  used

This is the clearest first consumer because the `%DATE_*%` and
`%TIME_*%` paths are already under coverage pressure.

### Runtime Timer And Delay Code

`src/runtime/sim_timer.c` already uses:

- `clock_gettime()`
- `nanosleep()`

but also still uses:

- `gettimeofday()`
- `sleep()`
- `ctime()`

This should be converted to:

- `sim_clock_gettime()`
- `sim_nanosleep()`
- `localtime_r()` / `gmtime_r()` and `strftime()` or direct formatting
  instead of `ctime()`

This is the main long-term reason to add the seam, because timeout and
elapsed-time logic should be testable against deterministic clock data.

This file also needs structural cleanup beyond simple API replacement:

- separate the POSIX and Windows time-backend code more clearly
- isolate the kernel-facing wrappers from the timer policy logic
- leave a few low-value legacy paths as explicit exceptions if they are
  not worth untangling immediately

### SIM_ASYNCH_CLOCKS

`SIM_ASYNCH_CLOCKS` is part of this cleanup because it affects how timer
events are driven.

From the current code:

- it is enabled only when asynchronous I/O support is available
- it allows clocks to run asynchronously via the wallclock queue and
  related timer-thread logic
- `sim_asynch_timer` represents whether asynchronous clock operation is
  active at runtime

The time-wrapper work should not try to redesign asynchronous clocks at
the same time, but it must not make that code harder to separate and
test later.

### SCP And Console Formatting

`src/core/scp.c` and `src/runtime/sim_console.c` currently use:

- `time()`
- `localtime()`
- `gmtime()`
- `mktime()`
- `ctime()`

These should be converted to:

- `sim_time()` or `sim_clock_gettime()` where they need "now"
- `localtime_r()` / `gmtime_r()` for breakdown
- `mktime()` can remain direct
- direct formatting or `strftime()` instead of `ctime()`

These are mostly formatting and logging paths and should be simpler to
migrate after the basic seam exists.

### Device And Simulator Callers

There is a long tail of device- and simulator-specific uses, including:

- many `localtime()` calls in clock and metadata paths
- many `time()` calls for timestamps and time-of-day emulation
- isolated `mktime()` / `difftime()` logic
- a few `sleep()` / `nanosleep()` sites in UI or panel code

These should be migrated later, after the common SCP/runtime code is
standardized.

Typical conversions will be:

- `time()` -> `sim_time()` or `sim_clock_gettime()` plus extraction of
  `tv_sec`
- `localtime()` -> `localtime_r()`
- `gmtime()` -> `gmtime_r()`
- `ctime()` / `asctime()` -> `strftime()` or direct formatting
- `sleep()` -> `sim_sleep()`
- `usleep()` -> `sim_nanosleep()`
- `gettimeofday()` -> `sim_clock_gettime()`

## Suggested Order

1. Add `src/runtime/sim_time.[ch]` with thin wrappers for:
   - `clock_gettime()`
   - `nanosleep()`
   - current-wallclock-seconds extraction
   - whole-second sleep convenience

2. Migrate `src/core/scp_cmdvars.c`.
   - this gives immediate value for `%DATE_*%` and `%TIME_*%` coverage

3. Migrate `src/runtime/sim_timer.c` and closely related delay code.
   - this is where the clock seam matters most
   - split the POSIX and Windows backend code more cleanly as part of
     that work

4. Migrate formatting/logging paths in `scp.c` and `sim_console.c`.

5. Migrate device- and simulator-specific callers opportunistically.

## Open Questions

- whether some UI or standalone utility sleep calls should remain direct
  rather than going through `sim_nanosleep()`
- whether any helper for common timestamp formatting is worth adding
  later, after the source-of-time seam is in place

## Definition Of Done

- common SCP/runtime code no longer calls raw kernel time/sleep APIs
  directly
- tests can force date/time boundary cases without depending on the real
  host clock
- timeout and elapsed-time code can be tested against deterministic
  clock data
- `sim_timer.c` is cleaner about backend separation and no longer mixes
  platform plumbing so deeply with timer policy
- remaining direct time-related calls are only pure timestamp-processing
  helpers or conscious low-value exceptions
