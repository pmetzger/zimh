# IBM PC / IBM PC XT Restoration Plan

## Goal

Restore the `ibmpc` and `ibmpcxt` simulators so that they:

- build cleanly under the current CMake build
- have working runtime functionality again
- can be added back to the maintained CMake simulator inventory
- have at least basic automated coverage

## Current Status

These simulators are not merely missing from CMake. They are presently out of
sync with the shared Intel common code in the tree.

The old top-level `Makefile` marked both targets with `#cmake:ignore-target`,
and current build attempts show that this was masking real code drift.

## Confirmed Breakages

### 1. Missing Header Dependency

`simulators/Intel-Systems/common/i8088.c` includes:

- `cpu.h`

There is no `cpu.h` anywhere in the repository.

This must be resolved before either simulator can build.

Possible resolutions:

- recover the missing header from the original source this code came from
- determine whether the include should actually point at a different checked-in
  header
- inline or replace the needed declarations if the missing header was only a
  thin shim

### 2. `system_defs.h` Drift

`simulators/Intel-Systems/ibmpc/system_defs.h` and
`simulators/Intel-Systems/ibmpcxt/system_defs.h` do not define macros now
expected by the shared Intel common code, including:

- `BYTEMASK`
- `DEBUG_xack`

The common code already assumes these exist in the Intel-MDS definitions, so
the IBM PC headers likely need to be brought up to the same baseline.

### 3. Helper API Signature Mismatch

`ibmpc.c` and `ibmpcxt.c` declare and call several helpers using signatures
that no longer match the shared common implementation.

Affected areas include:

- `i8237_reset`
- `i8253_reset`
- `i8255_reset`
- `i8259_reset`
- `EPROM_reset`
- `RAM_reset`
- `reg_dev`

This indicates that the simulator front-end code and the Intel common support
code diverged.

## Preserved Makefile Facts

The old top-level `Makefile` still records a few concrete details that should
be preserved while the legacy build logic is retired.

### Old Source Lists

`ibmpc` was built from:

- `simulators/Intel-Systems/common/i8255.c`
- `simulators/Intel-Systems/ibmpc/ibmpc.c`
- `simulators/Intel-Systems/common/i8088.c`
- `simulators/Intel-Systems/ibmpc/ibmpc_sys.c`
- `simulators/Intel-Systems/common/i8253.c`
- `simulators/Intel-Systems/common/i8259.c`
- `simulators/Intel-Systems/common/pceprom.c`
- `simulators/Intel-Systems/common/pcram8.c`
- `simulators/Intel-Systems/common/i8237.c`
- `simulators/Intel-Systems/common/pcbus.c`

`ibmpcxt` was built from:

- `simulators/Intel-Systems/common/i8088.c`
- `simulators/Intel-Systems/ibmpcxt/ibmpcxt_sys.c`
- `simulators/Intel-Systems/common/i8253.c`
- `simulators/Intel-Systems/common/i8259.c`
- `simulators/Intel-Systems/common/i8255.c`
- `simulators/Intel-Systems/ibmpcxt/ibmpcxt.c`
- `simulators/Intel-Systems/common/pceprom.c`
- `simulators/Intel-Systems/common/pcram8.c`
- `simulators/Intel-Systems/common/pcbus.c`
- `simulators/Intel-Systems/common/i8237.c`

These lists should be treated as the baseline source composition unless the
restoration work proves that one or more files no longer belong.

### Old Include-Path Intent

The old `Makefile` also recorded a local include-path expectation:

- `ibmpc` added `-I ${IBMPCD}`
- `ibmpcxt` added `-I ${IBMPCXTD}`

That suggests the local simulator directory was intended to provide the
primary `system_defs.h` view for each target.

### Old ROM-Build Dependency

Both `ibmpc` and `ibmpcxt` depended on `${BUILD_ROMS}` in the old
`Makefile`.

That should translate to `BUILDROMS` once these targets are restored in
CMake, unless the repair work shows that the ROM dependency is no longer
real.

### Old Test Hook Intent

Both targets had the generic `find_test(...)` hook in the old `Makefile`.

However, there do not appear to be checked-in `tests/*.ini` regression files
for these two simulators now. So this should be treated as evidence that the
targets were meant to participate in regression testing, not as proof that a
working test suite already exists.

### Old CMake Ignore Marker

Both targets were marked `#cmake:ignore-target` in the old `Makefile`.

That marker should be read as a warning that these simulators were already
known not to fit the old generator-based CMake path, not as a reason to leave
them broken indefinitely.

## Restoration Work

### Phase 1: Reconstruct The Intended Interface

Before making behavior changes, establish what the correct interfaces are
supposed to be now.

Tasks:

- inspect the current declarations and definitions in:
  - `simulators/Intel-Systems/common/`
  - `simulators/Intel-Systems/ibmpc/`
  - `simulators/Intel-Systems/ibmpcxt/`
- compare with the working Intel simulator families:
  - `Intel-MDS`
  - `scelbi`
- determine whether `ibmpc` and `ibmpcxt` should be adapted to the current
  common API, or whether the common API needs a compatibility shim

Deliverable:

- a short list of exact interface mismatches and the intended signatures for
  each one

### Phase 2: Resolve The Missing `cpu.h`

Tasks:

- determine what `i8088.c` expects from `cpu.h`
- locate the original source of that header, if possible
- otherwise replace the dependency with the minimum checked-in declarations
  needed for the current tree

Success criteria:

- `i8088.c` compiles without relying on a missing file

### Phase 3: Normalize IBM PC Local Definitions

Tasks:

- update `ibmpc/system_defs.h`
- update `ibmpcxt/system_defs.h`
- bring them to the same minimum compatibility baseline expected by the Intel
  common code

Likely additions:

- `BYTEMASK`
- `DEBUG_xack`

This phase should be kept small and explicit. Do not silently copy unrelated
  definitions unless they are actually required.

### Phase 4: Adapt IBM PC Front-End Code To Current Common Helpers

Tasks:

- fix the reset/helper declarations in `ibmpc.c`
- fix the reset/helper declarations in `ibmpcxt.c`
- update the call sites to match the current helper APIs
- verify that the runtime initialization logic still makes semantic sense

This is the phase most likely to reveal real functional regressions, not just
compile breakage.

### Phase 5: Add CMake Build Support

Once the code builds locally by hand under the current tree:

- add maintained local `CMakeLists.txt` files for:
  - `simulators/Intel-Systems/ibmpc`
  - `simulators/Intel-Systems/ibmpcxt`
- add those directories to `cmake/simh-simulators.cmake`
- use the existing Intel simulator pattern from:
  - `simulators/Intel-Systems/Intel-MDS/CMakeLists.txt`
  - `simulators/Intel-Systems/scelbi/CMakeLists.txt`

Do not do this before the code-level incompatibilities are fixed, or the
maintained CMake inventory will be left pointing at broken targets.

### Phase 6: Rebuild Functional Confidence

There do not appear to be existing in-tree regression `.ini` tests for
`ibmpc` or `ibmpcxt`.

So this phase should include:

- verifying that both simulators start
- confirming that their ROM/RAM/device initialization paths work
- identifying any existing boot or smoke scripts outside the usual
  per-simulator `tests/` convention
- if no tests exist, adding at least a basic noninteractive smoke test for
  each simulator

Desired minimum end state:

- `cmake --build build/release --target ibmpc ibmpcxt` passes
- `ctest --test-dir build/release -R '^simh-(ibmpc|ibmpcxt)$'` passes

## Recommended Execution Order

1. inventory and document the interface mismatches
2. fix the missing `cpu.h` dependency
3. update the IBM PC `system_defs.h` files
4. repair the helper declarations and call sites
5. verify manual build success
6. add maintained CMake target definitions
7. add smoke/regression coverage

## Risks

### Hidden Functional Drift

The compile failures are likely only the first layer. Even after the code
builds, the simulator behavior may still be wrong if the old front-end code
assumed older semantics for shared helpers.

### Incomplete Upstream Materials

If `cpu.h` was never checked in, recovering the correct declarations may
require source archaeology rather than a straightforward repair.

### Test Coverage Gap

These simulators may not have current automated tests at all. If so, build
restoration alone is not enough; we need at least minimal smoke coverage
before treating the restoration as complete.
