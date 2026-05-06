# Event Queue Delay Types

The simulator event queue still represents instruction-delay intervals
with raw `int32` values.  This is historically understandable because
SIMH queue entries are instruction counts and some queue-processing code
uses signed intermediate values for catch-up and overdue events.

As a modern interface, this representation is questionable.  Queue
delays are durations and are normally non-negative, while long
wall-clock delays can exceed `INT32_MAX` instructions before they are
bounded for the existing queue.  This creates repeated sanitizer hazards
at conversion boundaries and makes it harder to see which values are
ordinary pending delays, overdue deltas, or sentinel/error values.

We should audit whether the queue API should use a named delay type
instead of raw `int32`.  A likely direction is to separate:

- non-negative queued instruction delays;
- signed catch-up or overdue deltas;
- sentinel/status values.

These categories should be separate because they represent different
facts with different valid ranges.  A queued delay is a duration: run
this unit after some number of instructions.  That should normally be
non-negative.  Catch-up and overdue values are differences: the queue is
some number of instructions ahead of or behind the current execution
point.  Those values can legitimately be signed.  Mixing the two in a
raw `int32` makes it hard to tell whether a negative value is a valid
lateness calculation, an immediate-processing convention, a sentinel, or
a bug.

A better design would make conversions explicit.  Queue insertion would
take a non-negative delay type, catch-up arithmetic would use a signed
delta type, and any boundary where a signed delta becomes a queued delay
would be named, checked, and tested.

The core queue implementation and public activation surface are mostly:

- `src/core/sim_defs.h`: `UNIT.time`, `UNIT.usecs_remaining`,
  asynchronous queue fields such as `UNIT.a_event_time`, and the
  asynchronous activation macros.
- `src/core/scp.h`: public queue prototypes such as `sim_activate`,
  `_sim_activate`, `sim_activate_abs`, `sim_activate_notbefore`,
  `sim_activate_after`, `sim_activate_after_d`,
  `_sim_activate_after`, `sim_activate_time`,
  `_sim_activate_queue_time`, and `sim_activate_time_usecs`.
- `src/core/scp.c`: base instruction-event queue state and logic,
  including `sim_clock_queue`, `sim_interval`, `_sim_activate`,
  `sim_activate_abs`, `_sim_activate_queue_time`,
  `_sim_activate_time`, `sim_activate_time`, and
  `sim_process_event`.
- `src/runtime/sim_timer.h` and `src/runtime/sim_timer.c`: timer-aware
  activation and co-scheduling, including `sim_timer_activate`,
  `sim_timer_activate_after`, `_sim_timer_activate_time`,
  `sim_timer_activate_time_usecs`, `sim_clock_coschedule`,
  `sim_clock_coschedule_tmr`, timer assist units, and clock catch-up
  state.
- `src/runtime/sim_tmxr.h` and `src/runtime/sim_tmxr.c`: TMXR
  activation wrappers/macros such as `tmxr_activate`,
  `tmxr_activate_after`, and `tmxr_clock_coschedule_tmr`, which route
  multiplexer polling through the same event queue and timer APIs.

This would also touch many simulator call sites that pass instruction
counts or wall-clock delays into `sim_activate*`,
`sim_activate_after*`, and `sim_clock_coschedule*`.  It should therefore
be handled as an intentional design cleanup under broad unit and
integration coverage, not as an incidental sanitizer patch.

## Consumers

The event queue is not an internal-only runtime detail.  It is a core
simulator API used throughout `simulators/`.  A quick search finds
hundreds of simulator files calling `sim_activate`,
`sim_activate_abs`, `sim_activate_after`,
`sim_activate_after_abs`, `sim_activate_notbefore`, and
`sim_clock_coschedule*` directly.

Common consumer patterns include:

- device service routines reactivating themselves after `UNIT.wait`;
- character I/O and console polling loops;
- disk, tape, card, and communication device completion delays;
- clock and timer devices using calibrated instruction delays;
- wall-clock scheduling through `sim_activate_after*`;
- TMXR and runtime subsystems wrapping or forwarding activation calls.

Any event queue delay type migration therefore has to audit both shared
runtime code and simulator-specific timing code.  It should not assume
that all callers pass simple non-negative instruction counts, because
some code uses zero or negative delays to force immediate processing or
catch-up behavior.
