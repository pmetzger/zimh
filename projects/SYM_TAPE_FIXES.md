`sim_tape.c` is now substantially covered by host-side unit tests, but
it is not yet at literal 100% behavioral coverage.  The remaining work
falls into a few clear buckets.

1. Cover the real asynchronous path under `SIM_ASYNCH_IO`.

   The current unit tests cover only the synchronous fallback behavior of
   the `*_a` entry points.  They do not exercise the threaded I/O path in
   `sim_tape.c`, including:

   - dispatch through the async worker
   - callback delivery after queued work completes
   - queue/cancellation behavior
   - any ordering assumptions in the async state machine

   This is the largest remaining subsystem gap.

2. Add direct format-specific tests instead of relying only on
   `sim_tape_test()`.

   The built-in `sim_tape_test()` now gives useful indirect coverage for
   several formats, but it is still a broad self-test rather than a set of
   focused assertions.  To get closer to full coverage, add explicit tests
   for the formats with distinct behavior:

   - `AWS`
   - `P7B`
   - `TPC`
   - `TAR`
   - ANSI memory-tape variants

   Those tests should assert the actual semantics that differ from normal
   SIMH/STD tape files, not just that the format can be selected.

3. Add explicit `wreom`/`wreom_a` edge-case tests.

   The `wreomrw` path is covered, but the underlying `wreom` behavior still
   has format-specific branches that should be pinned down directly:

   - `P7B` rejection with `MTSE_FMT`
   - `AWS` end-of-medium handling
   - any `PNU`/position side effects that differ from ordinary writes

4. Add direct tests for thin spacing wrappers that are currently covered
   only indirectly.

   These wrappers are low risk, but they are still separate public entry
   points in `sim_tape.h` and should be pinned down if the goal is full
   API-level coverage:

   - `sim_tape_sprecsf`
   - `sim_tape_sprecsr`
   - `sim_tape_spfilef`
   - `sim_tape_spfiler`

   The current suite mostly exercises the richer helper APIs that they
   delegate to.

5. Decide whether to test or explicitly defer internal self-test failure
   modes.

   `sim_tape_test()` is now covered for:

   - success in a temporary working directory
   - rejection when the unit is already attached

   What is not covered are its internal failure paths for:

   - temporary tape-file generation
   - classification failures
   - per-format self-test failures

   Those may or may not be worth testing directly.  If not, this should be
   considered an intentional boundary rather than an accidental omission.

6. Preserve the current decision to keep callback-wrapper coverage small
   and deterministic.

   The current callback-wrapper test intentionally covers only a minimal
   stable subset of synchronous `*_a` behavior:

   - `sim_tape_wrgap_a`
   - `sim_tape_wrrecf_a`
   - `sim_tape_rewind_a`
   - `sim_tape_rdrecf_a`

   Earlier attempts to combine callback verification with file-mark and
   positioning semantics made the suite flaky.  If wrapper coverage is
   expanded later, it should be done in small deterministic tests, not one
   long scenario.

In short:

- the main substantive gap is real async I/O coverage
- the main non-async gap is direct format-specific testing
- everything else is incremental tightening of wrapper and edge-case APIs
