# Common Source Testing Adoption Plan

## Goal

Reach **100% test coverage of the common sources** over time, with
coverage used as both a planning tool and an enforcement tool.

For this plan, "common sources" means the shared host-side code under:

- `src/core/`
- `src/runtime/`
- `src/components/`

This goal does **not** include:

- simulator-specific source trees under `simulators/`
- generated build artifacts
- third-party code under `third_party/`

The plan assumes the working style described in [TESTING.md](TESTING.md):

- tests before behavior changes
- characterization tests when intended behavior is unclear
- seams before redesign
- small, protected edits

## Non-goals

- Do not try to "improve everything at once."
- Do not begin with broad warning cleanup across untested code.
- Do not start with giant refactors of `scp.c` or other monoliths.
- Do not require test code to be small or elegant at the expense of
  safety.

## Explicit test philosophy

We should be perfectly happy for tests to be **much larger than the
underlying production code** they protect.

That is not a smell in this project. It is often the correct tradeoff.

Reasons:

- The common code is old, shared, and behavior-sensitive.
- The test code is the long-term safety net for refactoring.
- Complex setup, fixtures, fakes, golden files, and coverage-oriented
  edge-case tests are justified if they reduce the risk of regressions.
- A small production helper may require a large test surface to cover:
  platform behavior, legacy edge cases, malformed input, historical
  compatibility, and differential behavior against old implementations.

The bias should be:

- make tests clear
- make tests exhaustive
- make tests deterministic
- accept large test bodies when they buy safety

## Framework and harness strategy

Chosen unit-test framework:

- `cmocka`

Status:

- selected
- installed

Why:

- native C
- lightweight
- integrates cleanly with `ctest`
- good fit for fixtures and small doubles

## Test architecture

The common-source test strategy should have four layers:

1. **Host-side unit tests**
   - small direct tests of shared functions and helpers
   - fast and deterministic

2. **Characterization tests**
   - pin down existing behavior before cleanup
   - especially important for legacy parsing and formatting code

3. **Golden-file and approval tests**
   - for large structured outputs and binary/text file formats

4. **Existing simulator regression tests**
   - remain the outer integration safety net
   - not a substitute for common-source unit tests

## Coverage policy

Coverage is a required planning and enforcement tool for the common
sources.

Use coverage to:

- identify untested files and functions
- find risk-heavy partially covered regions
- gate new work in already-adopted areas
- track progress toward full common-source coverage

Coverage is **not** enough by itself. Weak assertions and accidental
execution do not count as adequate tests.

Coverage targets:

- long-term target: `100%` line coverage on common sources
- strong preference for near-complete branch coverage where practical
- changed-lines coverage must be effectively `100%` in adopted areas

Coverage exclusions should be rare and explicit. If a function or branch
cannot be reasonably tested, document why.

## Coverage implementation plan

1. Add a dedicated coverage build configuration for host-side testing.
2. Generate per-file reports for:
   - `src/core/`
   - `src/runtime/`
   - `src/components/`
3. Publish coverage summaries into a stable project file under version
   control.
4. Track:
   - per-directory coverage
   - per-file coverage
   - uncovered functions
   - large partially covered files
5. Add a "changed-lines coverage" expectation for future cleanup work in
   adopted files.

Useful tooling candidates:

- `llvm-cov` / `llvm-profdata` with Clang
- `gcov` / `lcov` / `genhtml` with GCC

The exact tool should follow the installed toolchain, but coverage
reporting must become a regular part of the workflow.

## Directory layout

Add a dedicated host-side test tree:

- `tests/unit/`

Suggested structure:

- `tests/unit/core/`
- `tests/unit/runtime/`
- `tests/unit/components/`
- `tests/unit/support/`
- `tests/unit/data/`

Where:

- `support/` holds shared harness helpers, fakes, temp-file helpers,
  fixture code, byte-buffer utilities, and assertion helpers
- `data/` holds golden files and malformed test inputs

## Core operating rule

No cleanup of a common-source area should proceed without a local test
foothold in that area.

In practice:

- if the code is already unit-testable, write unit tests first
- if it is not unit-testable, write characterization tests at the
  nearest stable boundary
- then introduce the smallest seam needed
- then add more direct tests
- then and only then start structural cleanup

## Phased adoption plan

### Phase 0: Infrastructure

Objective:

- create the host-side unit-test harness
- wire it into `ctest`
- establish coverage reporting

Deliverables:

- `tests/unit/` tree
- one shared support library
- one example unit-test executable
- one coverage build and reporting recipe
- one coverage summary file committed to the repo

Commit boundaries:

1. add framework and CMake support only
2. add shared test support utilities only
3. add coverage configuration and reporting docs only
4. add first example unit test only

### Phase 1: Easy footholds in runtime file-format code

Objective:

- build confidence in areas that are naturally testable

Primary file targets:

- `src/runtime/sim_card.c`
- `src/runtime/sim_imd.c`
- selected isolated helpers from `src/runtime/sim_fio.c`
- selected isolated helpers from `src/runtime/sim_tape.c`
- selected isolated helpers from `src/runtime/sim_disk.c`

Test types:

- text/binary parsing
- malformed-input handling
- round-trip encode/decode
- CR/LF behavior
- checksums
- metadata handling

Milestone:

- first real common-source files under host-side unit test
- first meaningful coverage numbers for `src/runtime/`

Commit boundaries:

1. `sim_card.c` tests only
2. `sim_imd.c` tests only
3. `sim_fio.c` isolated helper tests only
4. `sim_tape.c` isolated helper tests only
5. `sim_disk.c` isolated helper tests only

### Phase 2: Extract pure helpers from hard common files

Objective:

- create seams in monolithic files without changing behavior

Primary file targets:

- `src/runtime/sim_timer.c`
- `src/runtime/sim_sock.c`
- `src/runtime/sim_tmxr.c`
- selected parts of `src/core/scp.c`

Approach:

- identify pure or mostly pure logic
- extract tiny helpers
- write characterization tests first
- add direct unit tests for extracted helpers

Milestone:

- first direct unit tests touching logic extracted from monolithic core
  files

Commit boundaries:

1. extraction-only commit for one helper
2. test-only commit for that helper
3. any semantic cleanup as a separate later commit

### Phase 3: Introduce seams for stateful subsystems

Objective:

- make stateful shared code testable without large rewrites

Targets:

- `sim_timer.c`
- `sim_sock.c`
- `sim_tmxr.c`
- selected `scp.c` subsystems

Seam types:

- fake clock providers
- file operation indirection
- socket operation indirection
- overridable wrappers
- narrow ops structs

Milestone:

- deterministic tests for state transitions and failure paths

Commit boundaries:

1. seam introduction only
2. characterization tests only
3. deeper behavior tests only
4. cleanup/refactor only after tests are green

### Phase 4: Components coverage

Objective:

- bring shared component code under the same safety model

Targets:

- `src/components/display/`
- other shared components used by multiple simulators

Approach:

- test pure helpers directly
- use approval tests for rendering-related formatted output where needed
- use fake backends for side effects

Milestone:

- all shared components have at least a characterization test foothold

### Phase 5: `scp.c` decomposition campaign

Objective:

- reduce the risk profile of `scp.c` by testing and extraction, not by
  direct broad rewrite

Approach:

- identify subdomains inside `scp.c`
- extract one helper cluster at a time
- characterize first, then test directly
- do not attempt "unit test all of `scp.c`" as one effort

Candidate subdomains:

- argument parsing helpers
- formatting helpers
- command decoding helpers
- save/restore helper logic
- independent utility routines

Milestone:

- measurable coverage and shrinking untested surface inside `scp.c`

## Backlog

### Infrastructure backlog

- [x] Choose and install the C unit-test framework (`cmocka`).
- [ ] Add `tests/unit/` CMake integration.
- [ ] Add shared test support library under `tests/unit/support/`.
- [ ] Add temp-file and temp-directory helpers.
- [ ] Add byte-buffer and file comparison helpers.
- [ ] Add fixture helpers for golden-file tests.
- [ ] Add a coverage build recipe.
- [ ] Add a coverage report generation script.
- [ ] Add a committed coverage summary file.

### Runtime backlog

- [ ] Add `sim_card.c` characterization tests.
- [ ] Add `sim_card.c` round-trip tests.
- [ ] Add `sim_card.c` malformed-input tests.
- [ ] Add `sim_imd.c` parse/serialize tests.
- [ ] Add `sim_imd.c` malformed-image tests.
- [ ] Add `sim_fio.c` isolated helper tests.
- [ ] Add `sim_tape.c` isolated helper tests.
- [ ] Add `sim_disk.c` isolated helper tests.
- [ ] Extract pure logic from `sim_timer.c`.
- [ ] Add fake-clock tests for `sim_timer.c`.
- [ ] Extract parse/validation helpers from `sim_sock.c`.
- [ ] Add helper tests for `sim_sock.c`.
- [ ] Extract line/state helpers from `sim_tmxr.c`.
- [ ] Add helper tests for `sim_tmxr.c`.

### Core backlog

- [ ] Identify first safe extraction target in `scp.c`.
- [ ] Add boundary characterization test for that area.
- [ ] Extract helper with no semantic change.
- [ ] Add direct unit tests for extracted helper.
- [ ] Repeat by `scp.c` subdomain until coverage is complete.

### Components backlog

- [ ] Inventory `src/components/` for testability.
- [ ] Classify pure helpers vs side-effect-heavy code.
- [ ] Add first display helper tests.
- [ ] Add approval-style tests where output is structured and large.

### Coverage backlog

- [ ] Record baseline coverage for all common-source files.
- [ ] Publish uncovered file list.
- [ ] Publish partially covered function list.
- [ ] Track coverage deltas after each adoption milestone.
- [ ] Add changed-lines coverage expectations for adopted files.

### Cleanup backlog tied to tests

- [ ] Fix `-Wstrict-prototypes` only in covered areas.
- [ ] Fix `-Wmissing-prototypes` only in covered areas.
- [ ] Fix safe `-Wunused-parameter` only in covered areas.
- [ ] Defer `-Wmissing-field-initializers` until each target area has
      stronger local tests.
- [ ] Treat `-Wimplicit-fallthrough`, `-Wundef`, and uninitialized-value
      warnings as semantic work requiring subsystem tests first.

## Milestones

### Milestone 1: Harness in place

Success criteria:

- unit-test framework installed
- `tests/unit/` builds under `ctest`
- coverage build works
- first example test committed

### Milestone 2: Runtime foothold

Success criteria:

- `sim_card.c` and `sim_imd.c` have real unit coverage
- runtime coverage report exists
- at least one golden-file path is in use

### Milestone 3: First hard-file seam

Success criteria:

- one seam introduced into `sim_timer.c`, `sim_sock.c`, `sim_tmxr.c`, or
  `scp.c`
- one extracted helper fully unit tested

### Milestone 4: Coverage map complete

Success criteria:

- every file in `src/core/`, `src/runtime/`, and `src/components/` has a
  recorded coverage status
- uncovered files are explicitly listed and prioritized

### Milestone 5: Majority adoption

Success criteria:

- most common-source files have direct unit tests or strong
  characterization tests
- warning cleanup begins only in adopted areas

### Milestone 6: Full common-source coverage

Success criteria:

- `100%` line coverage on common sources
- branch coverage reviewed for critical logic
- no common-source cleanup proceeds without matching tests

## File-target priority list

Highest priority:

- `src/runtime/sim_card.c`
- `src/runtime/sim_imd.c`
- `src/runtime/sim_timer.c`
- `src/runtime/sim_sock.c`
- `src/runtime/sim_tmxr.c`
- `src/core/scp.c`

Second tier:

- `src/runtime/sim_tape.c`
- `src/runtime/sim_disk.c`
- `src/runtime/sim_fio.c`
- `src/runtime/sim_scsi.c`
- `src/runtime/sim_console.c`
- `src/runtime/sim_video.c`

Third tier:

- `src/components/display/*`
- remaining shared helpers under `src/components/`

## Commit boundary rules

Keep commits narrow and purpose-specific.

Preferred commit shapes:

1. framework/infrastructure only
2. seam introduction only
3. characterization tests only
4. direct unit tests only
5. semantic cleanup only
6. coverage-report update only

Avoid combining:

- seam introduction
- behavior change
- warning cleanup
- broad refactoring

into one commit.

## Enforcement policy

Once a common-source file is adopted into unit testing:

- every future change to that file must come with tests
- changed lines in that file must be covered
- warning cleanup in that file must preserve or increase coverage

## Success condition

This effort succeeds when the common-source code can be changed under
test protection rather than by intuition.

The desired end state is not merely "some tests exist." The desired end
state is:

- complete common-source coverage
- repeatable local and CI execution
- safe extraction and cleanup of legacy code
- warning cleanup guided by tests instead of hope
