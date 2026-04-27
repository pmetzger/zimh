# Simulator In-Process Test Harness

## Goal

Create a reusable host-side test harness architecture that can test
simulator families in-process, using the real simulator modules and the
real core/runtime code, without driving everything through external
`.ini` scripts or extracting one tiny helper function at a time.

The long-term goal is to make simulator behavior directly testable at the
same boundary users and guest software observe: memory, registers,
device registers, interrupts, events, attached media, and simulator
commands. This should let us add high-confidence unit and integration
coverage for all simulator families while keeping production code clean.

## Why This Is Needed

Small pure helpers are useful in isolated cases, but they do not scale as
the primary testing strategy. If every warning cleanup or device fix
requires extracting another private helper into its own small source file,
the production tree will accumulate artificial modules whose main purpose
is test linkage rather than simulator design.

At the other extreme, out-of-process `.ini` integration tests are
valuable acceptance tests, but they are too coarse and slow for many
small behavioral contracts. They also make it hard to inspect detailed
machine state after a single operation.

The right long-term boundary is an in-process harness that links the real
simulator implementation and exposes stable test APIs for manipulating
and observing the simulated machine.

## Design Principles

- Test through guest-visible behavior whenever practical.
- Prefer stable simulator-machine APIs over private helper functions.
- Keep production changes aimed at improving simulator architecture, not
  at adding test-only special cases.
- Avoid test-only preprocessor surgery around large simulator source
  files.
- Keep out-of-process executable tests as acceptance coverage, not as the
  only testing tool.
- Support generated test cases for CPUs, devices, and command behavior.
- Make new harness APIs reusable across simulator families.

## Proposed Architecture

Create a common harness layer for simulator-family test binaries. A test
binary should be able to link a simulator family and then:

- initialize the simulator as the normal executable would;
- select or configure a model;
- reset the simulated machine;
- deposit and examine memory;
- deposit and examine registers;
- read and write device registers;
- attach and detach media;
- run for a bounded number of instructions or events;
- run until idle, halted, or an expected stop condition;
- inspect interrupts, device status, memory, registers, and stop reason;
- invoke selected SCP/core commands in-process where command semantics
  are the behavior under test.

The harness should be layered:

1. A common core harness for SCP/core/runtime initialization and command
   execution.
2. A simulator-family harness for CPU, memory, device, and model-specific
   setup.
3. Device or subsystem helpers where they represent real guest-visible
   operations rather than private implementation details.

For example, a PDP-11 harness might eventually provide:

```c
pdp11_test_init(model);
pdp11_test_reset();
pdp11_test_deposit_word(addr, value);
pdp11_test_read_word(addr);
pdp11_test_write_io(addr, value);
pdp11_test_read_io(addr);
pdp11_test_attach_disk(device_name, unit, image_path);
pdp11_test_run_until_idle();
pdp11_test_assert_interrupt(mask);
```

The exact names should be designed when implementing the first harness;
the important point is that tests should describe machine operations, not
private C implementation details.

## Relationship To Existing Test Types

### Unit Tests

Use in-process harness tests for precise behavior that can be checked
without launching a simulator process. These should be fast enough to run
frequently during warning cleanups and refactors.

### Integration Tests

Keep executable `.ini` tests for acceptance coverage: command-line
startup, script execution, package behavior, and realistic boot or
diagnostic scenarios. These tests prove the built simulator works as a
program.

### Characterization Tests

For risky cleanups, especially signedness and undefined behavior fixes,
the harness should support running the same generated cases against a
baseline build and a changed build, then comparing normalized final
state. This is especially important for CPU instruction behavior and
device edge cases.

## Implementation Plan

1. Survey how simulator executables are currently assembled by CMake.
   Identify the minimum common core/runtime objects and per-family source
   lists needed to link test executables.

2. Pick one pilot simulator family. PDP-11 is a good candidate because it
   has many devices and existing integration coverage, but VAX may be
   more valuable if the next high-risk work is CPU signedness cleanup.

3. Build the smallest in-process harness executable for the pilot
   family. The first milestone is simply construction, reset, and a
   trivial memory/register assertion.

4. Add a command/core API layer only where it simplifies tests. Avoid
   driving everything through string commands if direct stable APIs are
   clearer, but keep command execution available for behavior that is
   genuinely command-level.

5. Add one real device behavior test that would otherwise require an
   awkward helper extraction or a slow `.ini` script. Use this to prove
   the harness is solving the right problem.

6. Generalize only after the pilot is useful. Extract common harness
   pieces into shared test support, then add a second simulator family to
   validate that the abstractions are not PDP-11-specific.

7. Document the harness pattern in `projects/TESTING.md` or a maintainer
   testing document once the design has stabilized.

## First Useful Test Targets

Good early candidates:

- PDP-11 disk controller register/address behavior.
- PDP-11 Massbus write-check behavior.
- VAX individual instruction behavior after the VAX single-instruction
  API is decomposed.
- 3B2 CPU condition-code or branch behavior.
- Device attach/reset behavior where integration tests are currently too
  slow or too indirect.

## Risks And Open Design Questions

- Many simulator modules currently rely on globals. The harness should
  work with that reality first, then use the pain points to guide later
  cleanup.
- Some simulator source files may not link cleanly without the full
  executable object set. The pilot should document the minimum link set
  rather than hiding the problem with stubs.
- Direct global manipulation can make tests fragile. Prefer harness
  functions that express guest-visible operations.
- SCP command APIs may need cleanup to be pleasant in-process test
  surfaces. If so, create separate refactoring slices rather than
  burying that work in simulator tests.
- Baseline-versus-changed characterization testing will need a reliable
  way to run the same generated cases against two builds.

## Non-Goals

- Do not replace all executable integration tests.
- Do not extract every private helper solely for unit testing.
- Do not build large test-only forks of simulator source files.
- Do not depend on linker dead-stripping of unused functions to make
  tests link.
