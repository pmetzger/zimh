# Build Cleanup Plan

This file tracks the remaining build-system cleanup work after the
completed migration to direct-maintained CMake metadata and a thin
compatibility `Makefile`.

## Residual Work

### 1. Normalize Project CMake Option Naming

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

Migration approach:
- add positive replacement names first
- update docs to teach only the new names
- remove the old names after the transition

Success criteria:
- project-defined options follow a small, predictable naming vocabulary

### 2. Replace OS-Name Conditionals with Feature Detection

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

## Current Priority

- audit the remaining OS-name conditionals in `cmake/` and `src/`, then
  convert the clear feature-detection wins first
