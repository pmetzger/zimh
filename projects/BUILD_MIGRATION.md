# Build Migration Plan

## Goal

Replace the current dual `Makefile`/CMake build arrangement with a
CMake-first build system that is:

- the sole source of truth for simulator build metadata
- incrementally migrated without large breakage windows
- clean enough that experienced CMake users would consider it well-structured
- still compatible with `make foo` style invocations at the end, with
  `make` acting only as a thin compatibility front-end over CMake

## Current State

The repository already has a substantial CMake build, but the top-level
`Makefile` is still the source of truth for important simulator metadata.

The most important current dependency is:

- `cmake/generate.py` reads the top-level `Makefile`
- it generates simulator `CMakeLists.txt` files and
  `cmake/simh-simulators.cmake`
- many simulator `CMakeLists.txt` files still explicitly say that changes
  must be made in the top-level `Makefile` and then regenerated

So the real problem is not "CMake cannot build the tree." The problem is
"CMake is not yet the authoritative description of the tree."

## Phase 1 Inventory Baseline

This section records the concrete first-pass inventory of where the current
build still depends on the top-level `Makefile`.

### Primary Dependency Chain

The current dependency chain is:

- the top-level `Makefile` defines simulator source lists, compile flags,
  simulator groupings, and related metadata
- [cmake/generate.py](/Users/perry/proj/simh/cmake/generate.py:1) parses that
  `Makefile`
- the generator emits:
  - per-simulator `CMakeLists.txt`
  - `cmake/simh-simulators.cmake`
  - `cmake/simh-packaging.cmake`

This means the old `Makefile` is still an upstream metadata source for CMake,
even though CMake is the build engine for most actual development work.

### Generated Artifacts Still Tied to Makefile

The following generated CMake artifacts are still logically downstream of the
top-level `Makefile`:

- `cmake/simh-simulators.cmake`
- `cmake/simh-packaging.cmake`
- most simulator `CMakeLists.txt` files under `simulators/`
- some generated unit-test `CMakeLists.txt` files beneath simulator
  directories

Evidence:

- `cmake/generate.py` explicitly states that it generates simulator
  `CMakeLists.txt` files from the top-level `Makefile`
- `cmake/simgen/cmake_container.py` contains the generated-file banner used in
  simulator `CMakeLists.txt`
- many simulator `CMakeLists.txt` files still include the "edit the top-level
  Makefile, then run cmake/generate.py" warning banner

### Metadata Currently Extracted from Makefile

Based on `cmake/generate.py`, `cmake/simgen/cmake_container.py`, and the
generated outputs, the generator currently derives at least the following from
the top-level `Makefile`:

- simulator source-file lists
- simulator-local include directories
- simulator compile definitions
- simulator compile-time feature flags
- source macros that expand into source-file groups
- simulator names and simulator directories
- `all` and `experimental` simulator group membership
- test names associated with simulators
- whether a simulator depends on ROM-building support
- packaging family membership and install metadata

Important detail:

- the generator does not just copy flat source lists
- it walks Makefile target dependencies and compile actions
- it expands Make variables/macros and infers the simulator build shape from
  those actions

This is why the migration must start by moving the metadata authority, not by
simply deleting generated files.

### CMake Logic Already Owning Shared Build Behavior

CMake already owns a substantial amount of shared build logic. In particular:

- top-level feature options in `CMakeLists.txt`, such as:
  - `WITH_NETWORK`
  - `WITH_VIDEO`
  - `WITH_UNIT_TESTS`
  - `RELEASE_LTO`
  - `DEBUG_WALL`
- shared simulator construction helpers in:
  - `cmake/add_simulator.cmake`
- dependency discovery and optional third-party handling in:
  - `cmake/dep-locate.cmake`
  - `cmake/os-features.cmake`
  - related CMake support modules

So the migration problem is not that CMake lacks any build framework. The
remaining dependency is mostly about simulator inventory and simulator-specific
metadata.

### Makefile-Owned Behaviors Still Needing CMake-Native Ownership

The following categories still need to be fully owned by CMake rather than by
the top-level `Makefile`:

- simulator inventory
  - what simulators exist
  - where their source directories are
- per-simulator source composition
- per-simulator compile defines and include paths
- simulator grouping
  - especially `all` and `experimental`
- simulator test association metadata
- simulator packaging family metadata
- remaining developer entry-point conveniences now expressed only in the
  top-level `Makefile`

### Remaining Top-Level Makefile Convenience/Policy Surface

The top-level `Makefile` still exposes legacy user/developer controls such as:

- `NONETWORK`
- `NOVIDEO`
- `DEBUG`
- `LTO`
- `TESTS`

Some of these already have clean CMake equivalents. The migration should not
try to preserve the old names, but it does need to ensure that the useful
underlying capabilities are present and documented on the CMake side.

### Packaging Dependency

Packaging is also still downstream of generator-era data:

- `cmake/simgen/packaging.py` owns simulator-to-family packaging metadata
- `generate.py` emits `cmake/simh-packaging.cmake`

So packaging is not an independent cleanup. It is part of the same metadata
migration, and it belongs in the final phase once build/test ownership is
cleanly in CMake.

### First Incremental Migration Targets

The safest first migration slices appear to be:

1. stop treating simulator `CMakeLists.txt` files as generated artifacts
2. move `cmake/simh-simulators.cmake` to hand-maintained or manifest-driven
   CMake
3. migrate simulator metadata ownership out of `Makefile` and into the chosen
   hybrid CMake layout
4. only after that, replace the top-level `Makefile` with a thin compatibility
   wrapper

### Inventory Conclusion

The central fact established by this inventory is:

- the real blocker is not missing CMake functionality
- the real blocker is that simulator metadata authority still sits in the
  top-level `Makefile`, with CMake artifacts generated downstream of it

Therefore the first implementation phase after planning should be a metadata
authority migration, not a superficial wrapper or documentation change.

## What "Authoritative CMake Metadata" Means

This phrase just means:

- when someone changes how a simulator is built, there should be one obvious
  place to edit
- that place should be part of the CMake build, not the old `Makefile`
- CMake should not be regenerating itself from some other source

In practical terms, the "metadata" here is the build description for each
simulator and the project as a whole, such as:

- which source files belong to a simulator
- which include directories it needs
- which compile definitions or feature flags it needs
- whether it needs display/network/ROM support
- what tests belong to it

Today, too much of that information still lives in the top-level
`Makefile`, and CMake partly inherits it. The migration goal is to make
CMake itself the thing maintainers edit directly.

## End State

When this migration is complete:

- the build graph and simulator metadata live in CMake, not in the
  top-level `Makefile`
- generated simulator `CMakeLists.txt` files are gone
- `cmake/generate.py` is gone, or reduced to unrelated tooling if it still
  has value
- `make foo` works only as a compatibility layer that invokes CMake targets
- `BUILDING.md` and all simulator comments describe the CMake workflow as
  the primary and authoritative one
- the CMake structure is simple, explicit, and unsurprising:
  - clear target ownership
  - central reusable helpers
  - minimal global side effects
  - options that map cleanly to features

## Design Principles

1. CMake must become the source of truth before the `Makefile` is reduced.
2. Migrate metadata before deleting compatibility shims.
3. Prefer declarative simulator metadata over copy-pasted target logic.
4. Avoid regeneration steps in normal developer workflows.
5. Keep each step reviewable and reversible.
6. Do not break common developer entry points until replacements exist.

## Migration Strategy

The migration should happen in phases that are independently reviewable and
that each leave the repository in a working state.

### Phase 0: Freeze the Direction

Purpose:
- stop adding new dependencies on the top-level `Makefile`
- document the intended architecture before changing implementation

Work:
- add this project plan
- treat CMake as the preferred workflow in docs
- avoid any new "edit Makefile then regenerate CMake" patterns

Success criteria:
- no new build work is added only to `Makefile`
- plan is specific enough to drive the next phases

### Phase 1: Inventory the Current Build Authority

Purpose:
- identify exactly what metadata still comes from the `Makefile`
- avoid accidental feature regressions during migration

Work:
- inventory everything `cmake/generate.py` extracts from `Makefile`
- classify that metadata into:
  - simulator source lists
  - include directories
  - compile definitions
  - optional feature flags
  - ROM-generation dependencies
  - per-simulator tests
  - packaging/grouping metadata
- inventory top-level `Makefile` conveniences not yet represented in CMake

Outputs:
- a migration checklist of remaining `Makefile`-owned facts
- a clear list of generated files that must be retired

Success criteria:
- we can point to every remaining reason the CMake build depends on the
  `Makefile`

### Phase 2: Define the CMake Source-of-Truth Model

Purpose:
- decide what the authoritative CMake representation should look like

Preferred direction:
- keep target creation logic centralized in reusable helpers like
  `cmake/add_simulator.cmake`
- move simulator-specific metadata into explicit CMake declarations
- avoid per-simulator CMake boilerplate when a data-driven declaration is
  cleaner
- use the hybrid model as the final architecture
- use small hand-maintained per-simulator `CMakeLists.txt` files as the local
  declaration point

Candidate approaches:

Chosen model:

- each simulator directory keeps a small hand-maintained `CMakeLists.txt`
- that file acts as the local declaration point for simulator-specific build
  facts
- shared logic lives centrally in helper code such as
  `cmake/add_simulator.cmake`

This means:

- no giant central manifest as the main source of truth
- no separate per-simulator manifest format
- no generator-owned simulator `CMakeLists.txt`

Why this is the best fit here:

- it preserves locality: a maintainer looking at a simulator directory still
  finds its build entry point there
- it avoids giant repetitive per-simulator boilerplate
- it avoids one enormous central manifest becoming hard to read
- it fits how good CMake projects are usually organized:
  - small local `CMakeLists.txt`
  - shared helper functions/macros
  - clear target ownership

The local `CMakeLists.txt` files should be small, declarative, and boring.
They should mostly declare local data and pass it to shared helpers. They
should not grow large custom frameworks of their own.

Success criteria:
- we choose one authoritative metadata model
- no future step requires Makefile regeneration

### Phase 3: Stop Generating CMake from Makefile

Purpose:
- break the architectural dependency cleanly

Work:
- migrate `cmake/simh-simulators.cmake` to hand-maintained or manifest-driven
  CMake
- convert generated simulator `CMakeLists.txt` files into normal maintained
  files
- remove generator comments that instruct maintainers to edit `Makefile`
- ensure the top-level CMake build no longer needs `cmake/generate.py`

Important constraint:
- do this while preserving current target names and build outputs as much as
  practical

Success criteria:
- deleting or ignoring the top-level `Makefile` does not change what CMake
  can discover/build
- `cmake/generate.py` is no longer part of the normal build/update path

### Phase 4: Achieve Feature Parity for Build Controls

Purpose:
- ensure all meaningful build controls are represented in CMake

Likely items to audit:
- network on/off
- video on/off
- debug vs release behavior
- LTO
- unit tests
- ROM building/usage
- experimental simulator grouping
- per-simulator build selection

Work:
- map each remaining `Makefile` toggle to:
  - an existing CMake option
  - a new CMake option
  - or a deliberate removal with rationale
- ensure the CMake option surface is coherent and documented

Success criteria:
- no meaningful build mode still requires the top-level `Makefile`

### Phase 5: Move Legacy Convenience Workflows onto CMake

Purpose:
- preserve ergonomics while retiring the legacy backend

Work:
- define the supported command-line workflows for developers
- ensure CMake/CTest equivalents exist for:
  - building individual simulators
  - building common groups
  - running unit tests
  - running regression suites
- add any missing wrapper scripts if direct CMake/CTest use is too awkward

Success criteria:
- maintainers can do their real work without needing the old `Makefile`

### Phase 6: Replace the Top-Level Makefile with a Compatibility Wrapper

Purpose:
- preserve `make foo` without preserving Make as the real build system

Target design:
- keep a very small top-level `Makefile`
- it should:
  - configure a build directory if needed
  - invoke `cmake --build ... --target foo`
  - invoke `ctest` for test-oriented convenience targets
- it should not contain simulator metadata, compile flags, feature logic, or
  platform-detection policy

Rules for the compatibility `Makefile`:
- no source lists
- no compile definitions
- no host feature detection
- no target graph duplication
- no regeneration logic

Chosen implementation:
- use a small hand-written compatibility `Makefile`

Why:
- it is easier to review
- it is easier to constrain
- it is less likely to grow into a second real build system again
- autogenerated wrapper logic would reintroduce indirection where the goal is
  clarity

Success criteria:
- `make pdp11` works
- `make test` or equivalent works
- the `Makefile` becomes disposable compatibility glue

### Phase 7: Remove the Legacy Build Logic

Purpose:
- finish the migration cleanly

Work:
- delete the old top-level `Makefile` logic
- delete `cmake/generate.py` and any no-longer-needed support code
- remove stale references in docs/comments
- remove obsolete migration notes once the new structure is stable

Success criteria:
- there is exactly one real build system: CMake
- `make` is a wrapper, not a second implementation

### Phase 8: Replace OS-Name Conditionals with Feature Detection

Purpose:
- reduce reliance on hard-coded host OS names in the source tree
- prefer capability checks discovered at configuration time
- make the code more portable to new or less common platforms

Rationale:
- `#if defined(__linux__)`, `#if defined(__APPLE__)`, and similar guards are
  often only proxies for the real question, such as:
  - does the platform support a specific header
  - does a function exist
  - does a library call behave correctly
  - does a file-open mode or socket option work
- replacing those with feature checks usually makes the code clearer and more
  portable

Policy:
- prefer feature-based conditionals over OS-name conditionals where practical
- use configuration-time probes for capabilities and semantics
- only keep OS-name conditionals where the behavior is truly tied to a named
  platform family and cannot be expressed cleanly as a feature test

Examples of the desired direction:
- use a configured define like `SIMH_NO_FOPEN_X` rather than checking host OS
  names in `sim_fio.c`
- probe for headers, functions, types, constants, and behavioral semantics in
  CMake instead of assuming them from the host name

Important caveat:
- the goal is not to replace OS `#if`s with arbitrary CMake-generated
  booleans everywhere
- the goal is to express real capability checks
- if a condition is truly about "this is Windows-specific behavior" or "this
  is POSIX API structure", then a platform-family split may still be the
  cleanest representation

Success criteria:
- major portability decisions are feature-driven instead of host-name-driven
- adding support for a new platform usually means satisfying capability
  probes, not sprinkling new OS-name checks through the code
- the remaining OS-name conditionals are rare, justified, and easy to explain

## Incremental Execution Plan

Recommended implementation order:

1. Inventory generator inputs and outputs.
2. Choose the authoritative CMake metadata shape.
3. Convert one or two simulator directories as exemplars.
4. Convert `cmake/simh-simulators.cmake`.
5. Convert the rest of the simulator directories.
6. Replace `Makefile` metadata logic with compatibility wrappers.
7. Remove the generator and dead legacy build logic.
8. After the build migration stabilizes, begin replacing OS-name conditionals
   with configuration-time feature detection where it improves portability and
   clarity.

This order avoids a "flag day" rewrite.

## Quality Bar

The resulting CMake should be "exceptionally clean." Concretely, that
means:

- targets are explicit and easy to trace
- simulator-specific data is easy to find
- helper functions/macros are small and clearly scoped
- options are documented and behave consistently
- generated files are minimized or eliminated
- there is no ambiguity about where to make a build change

## Risks

### Hidden Makefile Semantics

Risk:
- the top-level `Makefile` may encode special cases that are not obvious from
  surface inspection

Mitigation:
- inventory before migration
- migrate a few representative simulators first

### Simulator-Specific Drift

Risk:
- moving from generated CMake to maintained CMake may accidentally change
  per-simulator flags

Mitigation:
- keep target names stable
- compare source lists/defines during migration
- validate with targeted builds and tests

### Wrapper Makefile Becoming Another Real Build System

Risk:
- the final compatibility `Makefile` could quietly accumulate real logic
  again

Mitigation:
- define strict limits for what it may do
- keep all real decisions in CMake

## Compatibility Policy

The final compatibility `Makefile` does not need to preserve every legacy
entry point or option.

Recommended policy:

- preserve common and reasonable user-facing targets, such as:
  - `make <simulator>`
  - `make all`
  - `make test` or another small set of obvious test entry points
- do not preserve obscure or low-value legacy convenience targets just for
  historical fidelity

For legacy option names, there are two possible approaches:

- strict approach:
  - require users to learn the CMake option names
  - example: use `-DWITH_VIDEO=Off` instead of old Makefile spellings
- translation approach:
  - allow the wrapper `Makefile` to accept a few common old variable names
  - translate them internally to the equivalent CMake configuration options

Examples:

- old style:
  - `make pdp11 NOVIDEO=1`
- translated wrapper behavior:
  - configure/build CMake with `-DWITH_VIDEO=Off`
- strict wrapper behavior:
  - reject `NOVIDEO=1` and require the CMake spelling instead

Chosen policy:

- keep the wrapper thin
- do not translate legacy Makefile option spellings
- prefer and document only the real CMake option names

This keeps the compatibility layer simple and avoids preserving a second
option language.

The preserved compatibility target set should start small:

- `make all`
- `make <simulator>`
- `make test`
- optionally a very small number of obvious additions such as
  `make unit-tests` if they remain genuinely useful

Anything beyond that should justify its existence.

## Build Directory Policy

In a typical modern CMake project, out-of-source builds are the norm, and it
is common to use one build directory per configuration when developers work
directly with CMake.

Examples of normal conventions:

- `build`
- `cmake-build`
- `build/debug`
- `build/release`

For this project, the chosen policy should be:

- the compatibility `Makefile` uses a default release-oriented build
  directory:
  - `build/release`
- developers using CMake directly should use separate out-of-source build
  directories such as:
  - `build/release`
  - `build/debug`
- the compatibility `Makefile` auto-configures `build/release` if needed

This is intentionally simple:

- it matches the repository's current habits and documentation
- it keeps the wrapper predictable
- it avoids teaching the wrapper multiple personalities

Debug and release builds should still be fully supported, but the wrapper
`Makefile` only needs to drive the default release-style build. Developers
can and should use separate explicit build directories when working directly
with CMake, for example:

- `build/release`
- `build/debug`

## Open Questions for the User

These choices affect the design and should be settled early.

1. Use the chosen local-declaration model:
   - each simulator directory keeps a small, hand-maintained
     `CMakeLists.txt`
   - shared target construction logic lives in central CMake helpers

2. Preserve important and reasonable `make` targets, but not every obscure
   legacy one. A later pass should define the exact preserved set.

3. Use a small hand-written compatibility `Makefile`, not an autogenerated
   one.

4. Do not translate legacy Makefile option spellings. Document and support
   only the real CMake option names.

5. Packaging/install flows are part of the migration, but as a final phase
   after build/test migration is complete.

6. The compatibility `Makefile` should:
   - auto-configure the build directory if needed
   - use `build/release` as its default build directory
   - drive only the default release-style build
   - leave debug/custom configurations to direct CMake usage

## Suggested Immediate Next Step

Do Phase 1 next:
- produce a concrete inventory of everything `cmake/generate.py` extracts from
  the top-level `Makefile`
- then choose the authoritative CMake metadata model before writing migration
  code
