# Build Migration Plan

## Goal

Finish the move from the old dual `Makefile`/CMake arrangement to a
CMake-first build with these properties:

- CMake is the only real build system
- simulator metadata is maintained directly in CMake
- `make foo` continues to work, but only as a thin compatibility front-end
- the project looks straightforward and conventional to experienced CMake
  users

## Current State

The major metadata migration is complete.

- simulator-local `CMakeLists.txt` files are maintained directly
- `cmake/simh-simulators.cmake` is maintained directly
- `cmake/simh-packaging.cmake` is maintained directly
- the legacy CMake generator code has been removed
- maintainer documentation now describes the direct-maintenance model

The remaining work is about:

- replacing the real logic in the top-level `Makefile`
- finishing workflow parity for common developer entry points
- cleaning up the project-defined CMake option surface
- continuing the move from OS-name conditionals to configuration-time
  feature checks where that improves portability and clarity

## Architecture

### Source of Truth

Build metadata lives directly in CMake.

- each simulator directory has a small hand-maintained `CMakeLists.txt`
- shared target construction logic lives in common helpers such as
  `cmake/add_simulator.cmake`
- simulator packaging membership is declared with `PKG_FAMILY` in the local
  simulator file and matched by definitions in `cmake/simh-packaging.cmake`

This is intentionally simple:

- no generator-owned simulator metadata
- no separate manifest format
- no giant central metadata table

### Build Directories

The build tree policy is:

- `build/release` for the default release-style build
- `build/debug` for debug work

The future compatibility `Makefile` should auto-configure and use
`build/release`.

## Remaining Phases

### Phase 1: Workflow Parity

Purpose:
- make sure the useful day-to-day entry points have clean CMake-native
  equivalents

Status:
- partly complete

Already covered:
- building the default simulator set:
  - `cmake --build build/release`
- building an individual simulator:
  - `cmake --build build/release --target pdp11`
- building the experimental simulator set:
  - `cmake --build build/release --target experimental-simulators`

Still to do:
- define the supported test-oriented workflows that the compatibility
  `Makefile` will expose
- decide whether we want a dedicated convenience target for host-side unit
  tests, or whether documented `ctest` entry points are sufficient
- decide whether we want a convenience target for broader regression runs, or
  whether documented `ctest` usage is enough

Success criteria:
- every preserved user-facing `make` target has a clean CMake/CTest-side
  equivalent first

### Phase 2: Replace the Top-Level Makefile

Purpose:
- reduce the top-level `Makefile` to thin compatibility glue

Design:
- keep a small hand-written `Makefile`
- it may:
  - configure `build/release` if needed
  - call `cmake --build` for build targets
  - call `ctest` for test-oriented targets
- it must not:
  - carry simulator source lists
  - carry compiler flags or feature-selection logic
  - duplicate platform detection
  - implement a second target graph

Compatibility scope:
- preserve only common and useful entry points
- planned preserved set:
  - `make all`
  - `make <simulator>`
  - `make test`
- possible additional target:
  - `make unit-tests`
  only if it remains genuinely useful after the workflow cleanup

Option policy:
- the wrapper should not translate old Makefile option names
- users should use the actual CMake option names

Success criteria:
- `make` remains usable as a front-end
- CMake remains the only real build system

### Phase 3: Normalize Project CMake Option Naming

Purpose:
- make project-defined options look more like a coherent modern CMake
  project

Policy:
- keep standard CMake variables unchanged
  - for example, `CMAKE_BUILD_TYPE` stays `CMAKE_BUILD_TYPE`
- use `WITH_*` for product/runtime features
- use `ENABLE_*` for tooling and analysis features
- avoid negative option names such as `NO_*` and `DONT_*`
- do not introduce `WITHOUT_*`

Likely cleanup targets:
- `DONT_USE_ROMS`
- `NO_DEP_BUILD`

Migration approach:
- add positive replacement names first
- update docs to teach only the new names
- remove the old names after the transition

Success criteria:
- project-defined options follow a small, predictable naming vocabulary

### Phase 4: Replace OS-Name Conditionals with Feature Detection

Purpose:
- prefer capability checks over hard-coded host-name checks where that is the
  real requirement

Policy:
- use configuration-time probes for headers, functions, types, constants, and
  behavioral semantics when that expresses the real dependency
- keep platform-family checks only where the behavior is truly tied to a
  platform family and cannot be expressed cleanly as a feature test

Example direction:
- use a configured define such as `SIMH_NO_FOPEN_X` for a tested capability
  instead of branching on host names in the implementation

Success criteria:
- portability decisions are mainly feature-driven
- remaining OS-name conditionals are rare and clearly justified

## Build Control Mapping

The useful old top-level `Makefile` controls already map fairly cleanly to
CMake.

- `NONETWORK=1`
  - `-DWITH_NETWORK=Off`
- `NOVIDEO=1`
  - `-DWITH_VIDEO=Off`
- `DEBUG=1`
  - `-DCMAKE_BUILD_TYPE=Debug`
- `LTO=1`
  - `-DRELEASE_LTO=On`
- `DONT_USE_ROMS=1`
  - currently `-DDONT_USE_ROMS=On`
  - should eventually become a positive-name option

Items that still need policy decisions:

- test-running convenience entry points
- any compatibility handling for warnings policy knobs
- whether any optimization-control convenience option should exist at all

## Quality Bar

The end state should be boring in a good way.

- simulator-specific metadata is easy to find
- shared logic is centralized and limited in scope
- target names and ownership are easy to trace
- options are named consistently and documented clearly
- there is no ambiguity about where to change build behavior

## Immediate Next Step

The next concrete work item is:

- define and implement the CMake/CTest-side workflows that will back the
  preserved test-oriented `make` targets

That means deciding what `make test` and any optional `make unit-tests`
compatibility target should actually do, implementing the underlying
CMake/CTest workflow cleanly, and only then replacing the top-level
`Makefile` logic.
