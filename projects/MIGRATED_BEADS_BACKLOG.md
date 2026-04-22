# Migrated Beads Backlog

This file preserves the content of the unclosed Beads issues that were
present when repo-local Beads support was removed.

Some of these items may already be partly or fully addressed and should be
triaged before further work.

## simh-0i1

Status: `in_progress`

Title: `Deepen unit coverage for src/runtime/sim_tape.c beyond foothold tests`

Why this exists:
- a first host-side unit-test foothold exists for `sim_tape.c`
- current coverage is limited to predicates and set/show helpers

Foothold already landed:
- BOT/EOT/write-protect predicates
- format, capacity, density helpers
- error text mapping

What remains:
- deepen coverage into more meaningful tape-format and behavior logic
- cover additional edge cases and state transitions
- stop and ask if further progress requires non-obvious `src/core`
  refactoring

Acceptance criteria:
- `sim_tape.c` coverage is materially beyond foothold status
- targeted unit tests cover substantive tape behavior, not just set/show
  helpers
- targeted build and test runs pass

Dependencies:
- depends on `simh-0mm` `Deepen unit coverage for src/runtime/sim_fio.c beyond foothold tests`
- blocks `simh-c2g`

## simh-2wwc

Status: `open`

Title: `Deepen unit coverage for src/runtime/sim_tmxr.c beyond foothold tests`

Why this exists:
- a first host-side unit-test foothold exists for `sim_tmxr.c`
- the file should stay open until coverage extends into more meaningful
  multiplexer behavior

Foothold already landed:
- loopback/half-duplex helpers
- queue-length helpers
- poll-interval validation

What remains:
- deepen coverage into more meaningful line-state or packet/buffer behavior
- stop and ask if further progress requires non-obvious `src/core`
  refactoring

Acceptance criteria:
- `sim_tmxr.c` coverage is materially beyond foothold status
- targeted unit tests cover substantive multiplexer behavior beyond simple
  flag helpers
- targeted build and test runs pass

Dependencies:
- depends on `simh-4er`

## simh-4er

Status: `open`

Title: `Deepen unit coverage for src/runtime/sim_sock.c beyond foothold tests`

Why this exists:
- a first host-side unit-test foothold exists for `sim_sock.c`
- current coverage covers address parsing and ACL checks only

Foothold already landed:
- `sim_parse_addr`
- `sim_parse_addr_ex`
- `sim_addr_acl_check` basic cases

What remains:
- add more parser edge cases, validation behavior, and ACL matching cases
- stop and ask if deeper coverage requires non-obvious `src/core`
  refactoring

Acceptance criteria:
- `sim_sock.c` coverage is materially beyond foothold status
- targeted unit tests cover parser and ACL edge cases more thoroughly
- targeted build and test runs pass

Dependencies:
- depends on `simh-8w2`
- blocks `simh-2wwc`

## simh-8w2

Status: `open`

Title: `Deepen unit coverage for src/runtime/sim_timer.c beyond foothold tests`

Why this exists:
- a first host-side unit-test foothold exists for `sim_timer.c`
- current coverage covers timespec math and default-state timer queries

Foothold already landed:
- `sim_timespec_diff`
- default-state timer query helpers

What remains:
- deepen coverage into more meaningful scheduling/calibration logic where
  possible
- if that requires a new seam or extraction, stop and ask before doing
  non-obvious `src/core` refactoring

Acceptance criteria:
- `sim_timer.c` coverage is materially beyond foothold status, or a clearly
  justified seam plan is agreed
- targeted build and test runs pass

Dependencies:
- depends on `simh-c2g`
- blocks `simh-4er`

## simh-8x5c

Status: `open`

Title: `Refactor TMXR attach-string assembly onto sim_dynstr helpers`

Description:
- replace the generic local dynstr helpers in `sim_tmxr` attach-string
  assembly with the shared `sim_dynstr` APIs now available in `src/runtime`
- keep behavior unchanged under the existing TMXR tests
- continue the TMXR cleanup work from there

## simh-c2g

Status: `open`

Title: `Deepen unit coverage for src/runtime/sim_disk.c beyond foothold tests`

Why this exists:
- a first host-side unit-test foothold exists for `sim_disk.c`
- current coverage covers format/capacity/status helpers only

Foothold already landed:
- format helpers
- capacity helpers
- simple status predicates

What remains:
- deepen coverage into more meaningful disk behavior and edge cases
- stop and ask if further progress requires non-obvious `src/core`
  refactoring

Acceptance criteria:
- `sim_disk.c` coverage is materially beyond foothold status
- targeted unit tests cover substantive disk logic beyond simple helper
  routines
- targeted build and test runs pass

Dependencies:
- depends on `simh-0i1`
- blocks `simh-8w2`

## simh-su35

Status: `open`

Title: `Continue TMXR string-safety cleanup after dynstr refactor`

Description:
- pick the next safe `sim_tmxr` cleanup slice after the dynstr refactor
- focus on reporting or configuration paths already covered by tests
- avoid broader host-I/O refactors until justified
- record any new cleanup opportunities discovered along the way
