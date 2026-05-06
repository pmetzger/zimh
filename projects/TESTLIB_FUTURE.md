# TESTLIB Future

## Current Findings

Manual experiments with `zimh-pdp11` show that `TESTLIB` needs
renovation before we can safely replace it with automated unit and
integration test coverage.

- `TESTLIB TQ` currently works as a useful tape-library smoke test. It
  runs `sim_tape_test()` against the PDP-11 `TQ0` device, creates and
  processes several temporary tape images, exercises the major tape image
  formats, exits cleanly, and removes its generated files.
- Bare `TESTLIB` is not reliable as an all-device test in the current
  environment. It starts SCP self-tests, skips non-library devices, then
  fails on the PDP-11 `DLI` device because `tmxr_sock_test()` cannot
  bind `localhost:65500` in the sandboxed environment.
- The simulator process exited with status 0 even when bare `TESTLIB`
  reported a library-test failure internally. Any automated use needs to
  check simulator output or improve command/process status propagation.
- Sanitized simulator binaries may fail before `TESTLIB` runs. In one
  trial, the sanitizer build aborted at startup on an existing
  `sim_timer.c` undefined-behavior finding, so the experiment used a
  non-sanitizer debug build.
- Because tests are dispatched through real simulator devices, results
  depend on the selected simulator's configured devices and on host
  resources such as sockets and writable current directories.

These findings suggest that the next step is coverage migration, not
immediate removal. First, make the relevant behavior predictable, isolate
environment-sensitive cases, and add automated command-level coverage
for the parts we still want to preserve.

`TESTLIB` is a built-in simulator command that runs internal device
library tests from a simulator prompt. It currently has value as a
developer smoke test because it exercises library behavior through real
simulator devices, command parsing, attach/detach paths, switches,
current working directory behavior, and device enable/disable plumbing.

It should not remain the primary assurance mechanism for library
behavior. The detailed checks inside `sim_tape_test()`, `sim_disk_test()`,
`sim_card_test()`, `tmxr_sock_test()`, and similar routines should
migrate toward the normal automated test infrastructure.

## Goals

- Move detailed behavior checks into deterministic unit or integration
  tests that run in CI.
- Keep changed-line coverage strong when modifying shared device
  libraries.
- Preserve useful end-to-end coverage of simulator command paths such
  as `ATTACH`, `DETACH`, and `TESTLIB`.
- Avoid relying on tests that create ad hoc files in the caller's
  current working directory unless the test harness controls that
  directory.

## Options

### Keep TESTLIB Temporarily As A Diagnostic Wrapper

Keep the command for now, but treat it as a developer convenience and
smoke test while the detailed checks move into the normal automated test
suite. The implementation can eventually call smaller helpers that are
already covered by unit tests.

This preserves an easy manual check for simulator maintainers while
moving the real assurance into CI. Once equivalent automated coverage
exists, we should reassess whether the command still earns its keep.

### Convert TESTLIB Scenarios Into Integration Tests

Add automated integration tests that launch a simulator and run commands
such as:

```text
TESTLIB TAPE
TESTLIB -d TAPE
```

This keeps coverage of the user-visible command path without depending
on humans to invoke it manually.

### Deprecate TESTLIB After Coverage Migration

If the command becomes redundant, hard to maintain, or confusing, we
should deprecate it after unit and integration tests cover the same
behavior. This should only happen after documenting what coverage
replaced it.

## Near-Term Direction

Use the automated test suite as the source of truth. When touching code
covered only by `TESTLIB`, first add unit or integration coverage around
the relevant behavior, then refactor or repair the library code.

For `sim_tape.c`, continue moving testable pieces out of the built-in
self-test and into `tests/unit/src/runtime/test_sim_tape.c` or an
integration test that runs a real simulator command script.
