# Host And Guest Time Separation

## Problem

The simulator currently has several notions of time that are easy to
confuse:

- host wall-clock time, used to protect test harnesses and interactive
  users from a hung simulator
- guest-visible device time, used by simulated interval timers,
  clocks, and time-of-day devices
- instruction-count time, used by the main event queue and by
  diagnostic acceleration and other virtualized-time policies
- calibrated host/guest timing, used to make simulated timer devices
  track real elapsed host time during normal operation

These clocks must not be interchangeable.  A test harness timeout must
continue to measure real host time even when a guest-visible clock is
intentionally distorted, accelerated, slowed, or driven from an
instruction-count policy.

## Current Evidence

The `RUNLIMIT n SECONDS|MINUTES|HOURS` command looks like a host
wall-clock guard in the command documentation and in diagnostic scripts,
but it is scheduled with `sim_activate_after_d()` on the simulator event
queue.  That path goes through `sim_timer_activate_after()`, converts
microseconds to instruction/event scheduling using the currently
calibrated timer state, and can be affected by guest timer policy.

That is not a safe harness watchdog for tests that deliberately distort
guest-visible time.  In diagnostic acceleration experiments, replacing
instruction runlimits with `RUNLIMIT 2 MINUTES` caused diagnostics to
hit the runtime limit after only a few seconds of host time once a
model-specific clock-calibration workaround was removed.  That is the
symptom we want to fix in the core time/runlimit design, not by adding
model-specific clock-calibration exceptions.

## Desired Architecture

The code should make the time domain explicit at every boundary:

- host-watchdog operations should use an independent host monotonic
  clock and should not be scheduled through guest/event time
- guest-visible timer and clock devices may use calibrated event time,
  instruction-count time, or model-specific policies as appropriate
- test-harness timeouts should remain correct even if guest time is
  sped up, slowed down, frozen, or otherwise virtualized
- command names, help text, and project scripts should not call an
  event-queue timeout a wall-clock timeout unless it is actually backed
  by host monotonic time

## Candidate Work

1. Audit all users of time-unit `RUNLIMIT` and document whether they
   mean host watchdog time or guest/event time.
2. Add focused tests around `RUNLIMIT n SECONDS|MINUTES|HOURS` that
   characterize the current behavior before changing it.
3. Create a seam around the host monotonic clock so `RUNLIMIT` behavior
   can be tested without sleeping in unit tests.
4. Decide whether to change existing time-unit `RUNLIMIT` to be a real
   host watchdog or to introduce a new explicitly host-clock command.
5. If changing existing `RUNLIMIT`, update help text and diagnostic
   scripts after the new behavior is covered by tests.
6. Keep instruction-count `RUNLIMIT n INSTRUCTIONS` as an event-queue
   mechanism; it is intentionally tied to guest instruction execution.

## Test Requirements

Before changing production code:

- add unit coverage proving instruction-count runlimits remain event
  queue based
- add unit coverage proving time-unit runlimits use the intended time
  domain
- add tests where guest/event time advances quickly while host time does
  not, proving a host watchdog does not expire early
- add tests where host time advances while guest/event time is stalled,
  proving a host watchdog still expires
- update at least one affected integration harness only after the core
  semantics are covered

## Relationship To Diagnostic Acceleration

Diagnostic acceleration exposes this problem because it can deliberately
change how diagnostic-visible timer delays map to instruction counts.
Model-specific projects should not carry local workarounds for
`RUNLIMIT` semantics.  Until host and guest time are separated properly
in the core, diagnostic experiments should avoid relying on
simulator-side time-unit `RUNLIMIT` as a real host-time safety guard.
