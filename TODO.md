# Repository Layout Restructuring Plan

## Goals

Restructure the repository so that the top level is small, legible, and
purpose-driven. The root should contain only a few project entrypoints and
policy files, while source, simulators, documentation, third-party code,
and tooling live in predictable locations.

This plan assumes:

- a clean-break migration is acceptable
- current simulator family names and casing are preserved for now
- documentation already living inside simulator subdirectories stays there
  for now
- VMS host-build support can be removed
- out-of-tree builds become the supported model

## Target Top-Level Layout

The intended end-state root should be approximately:

```text
/
  README.md
  AGENTS.md
  TODO.md
  LICENSE.txt
  .gitignore
  .gitattributes
  .editorconfig
  CMakeLists.txt
  Makefile
  vcpkg.json
  cmake/
  src/
  simulators/
  docs/
  third_party/
  tools/
  tests/
```

Notes:

- `CMakeLists.txt`, `Makefile`, `cmake/`, and `vcpkg.json` stay visible at the
  top level because they are primary project entrypoints.
- `src/` becomes the home for shared SIMH core code and auxiliary in-tree
  components.
- `simulators/` becomes the home for simulator-family source trees.
- `docs/` becomes the home for top-level documentation, converted manuals, and
  historical notes that do not belong beside a specific simulator.
- `third_party/` becomes the home for vendored or externally-derived code.
- `tools/` becomes the home for scripts and legacy-but-still-supported build
  helpers.
- `tests/` becomes the home for top-level/shared test infrastructure.

## Phase 1: Classify and Freeze the Current Tree

Before moving anything:

- create a written inventory of every current top-level file and directory,
  grouped as:
  - project entrypoint
  - shared runtime/core source
  - simulator family
  - third-party code
  - build system/tooling
  - CI support
  - documentation
  - generated/build output
  - legacy/obsolete
- add a temporary restructuring guide in the contributor docs describing the
  target layout and stating that moves are in progress
- verify all active build and test entrypoints before the first move:
  - CMake configure/build/test on Linux or macOS
  - top-level `Makefile` build smoke test
  - Windows CMake workflow sanity check from CI definitions

Acceptance criteria:

- every current top-level entry has an explicit destination or a removal
  decision
- no remaining "miscellaneous" bucket

## Phase 2: Define Destination Ownership

### 2.1 `src/`

Move top-level shared code into `src/`, with substructure rather than a flat
dump:

- `src/core/`
  - `scp.c`, `scp.h`, `scp_help.h`
  - `sim_defs.h`
  - `sim_printf_fmts.h`
  - `sim_rev.h`
- `src/runtime/`
  - `sim_console.*`
  - `sim_disk.*`
  - `sim_fio.*`
  - `sim_sock.*`
  - `sim_timer.*`
  - `sim_tmxr.*`
  - `sim_card.*`
  - `sim_tape.*`
  - `sim_serial.*`
  - `sim_imd.*`
  - `sim_scsi.*`
  - `sim_ether.*`
  - `sim_video.*`
  - `sim_frontpanel.*`
  - `sim_BuildROMs.c`
- `src/components/`
  - current `display/`
  - current `frontpanel/`
  - current `stub/`
  - current `helpx` material if retained as source/tooling rather than docs

Rules:

- do not keep any shared `.c` or `.h` files in the repository root after this
  phase
- define and document public include paths explicitly
- update all build logic to consume the new paths from variables rather than
  stringly-typed literals spread through the tree

### 2.2 `simulators/`

Move simulator family directories under `simulators/` while preserving names:

- `simulators/3B2`
- `simulators/ALTAIR`
- `simulators/AltairZ80`
- `simulators/B5500`
- `simulators/BESM6`
- `simulators/CDC1700`
- `simulators/GRI`
- `simulators/H316`
- `simulators/HP2100`
- `simulators/HP3000`
- `simulators/I1401`
- `simulators/I1620`
- `simulators/I650`
- `simulators/I7000`
- `simulators/I7094`
- `simulators/Ibm1130`
- `simulators/Intel-Systems`
- `simulators/Interdata`
- `simulators/LGP`
- `simulators/ND100`
- `simulators/NOVA`
- `simulators/PDP1`
- `simulators/PDP10`
- `simulators/PDP11`
- `simulators/PDP18B`
- `simulators/PDP8`
- `simulators/PDQ-3`
- `simulators/S3`
- `simulators/SAGE`
- `simulators/SDS`
- `simulators/SEL32`
- `simulators/SSEM`
- `simulators/TX-0`
- `simulators/VAX`
- `simulators/alpha`
- `simulators/imlac`
- `simulators/linc`
- `simulators/sigma`
- `simulators/swtp6800`
- `simulators/tt2500`

Rules:

- preserve simulator-local tests and simulator-local documentation in place
- keep existing simulator family names and casing
- do not mix shared runtime code into simulator trees during the move

### 2.3 `docs/`

Create a structured documentation tree and empty the root of historical docs:

- `docs/project/`
  - `README-CMake.md`
  - `SIMH-SG.md`
  - `SIMH-V4-status.md`
- `docs/history/`
  - `0readmeAsynchIO.txt`
  - `0readme_39.txt`
  - `0readme_ethernet.txt`
- `docs/legacy-word/`
  - current contents of `doc/`
- `docs/build/`
  - build and dependency notes that are currently split across READMEs

Rules:

- keep simulator-specific READMEs with their simulator directories for now
- do not leave project documentation scattered between root and `doc/`
- explicitly mark Word-format content as legacy and slated for conversion

### 2.4 `third_party/`

Move vendored or externally-derived code under `third_party/`:

- `third_party/slirp/`
- `third_party/slirp_glue/`

Investigate whether `display/` belongs in `src/components/` or `third_party/`.
Default for now: treat `display/` as an in-tree component and place it under
`src/components/display/` unless provenance review shows it should be
quarantined as third-party.

### 2.5 `tools/`

Create a tooling area to get host/build clutter out of the root:

- `tools/build/`
  - `build_mingw.bat`
  - `build_mingw_ether.bat`
  - `build_mingw_noasync.bat`
  - `build_vstudio.bat`
- `tools/ci/`
  - rename current `.travis/` to a saner name here, e.g. `tools/ci/deps/`
  - move CI helper scripts here and update GitHub Actions to call the new paths
- `tools/dev/`
  - one-off developer scripts and migration helpers

Rules:

- no platform-specific helper batch files at the repository root
- no CI-support directory with a historical brand name like `.travis/`

### 2.6 Legacy/Obsolete Items

Treat these separately:

- remove `descrip.mms` as part of the host-support reduction work
- likely remove `.travis.yml`
- likely remove `appveyor.yml`
- review `Visual Studio Projects/`
  - preferred end-state is removal after confirming CMake fully replaces it
  - if immediate removal is too risky, quarantine it under
    `tools/legacy/visual-studio-projects/` first and remove later

## Phase 3: Build-System Migration Work

The layout change will fail unless the build systems are made path-agnostic.
Do this before or alongside the directory moves.

### 3.1 CMake

- replace hardcoded top-level simulator and source paths with structured path
  variables
- define canonical roots such as:
  - `${SIMH_SOURCE_ROOT}`
  - `${SIMH_CORE_ROOT}`
  - `${SIMH_SIMULATOR_ROOT}`
  - `${SIMH_THIRD_PARTY_ROOT}`
  - `${SIMH_TOOLS_ROOT}`
  - `${SIMH_DOCS_ROOT}`
- update `cmake/generate.py` and related generation logic to emit paths under
  `simulators/` and `src/`
- update every simulator `CMakeLists.txt` to consume the new root layout
- ensure generated files do not bake in old top-level paths

### 3.2 Makefile

- update the top-level `Makefile` to reference:
  - shared core under `src/`
  - simulator families under `simulators/`
  - helper scripts under `tools/`
- remove assumptions that source files live in the root
- document the `Makefile` as a legacy-but-supported build path unless and until
  the project decides otherwise

### 3.3 Out-of-Tree Build Policy

- make out-of-tree builds the documented and supported default
- ensure `.gitignore` covers all build output directories
- stop relying on root-level generated artifacts where possible
- decide whether `.git-commit-id` and `.git-commit-id.h` should move under a
  build directory or a generated-files area rather than the root

Acceptance criteria:

- a fresh clone can build without generating tracked clutter in the root
- both CMake and `Makefile` entrypoints work from the new layout

## Phase 4: Migration Sequence

Perform the actual restructuring in controlled commits.

Recommended order:

1. Introduce path variables and refactor build logic without moving files yet.
2. Create destination directories.
3. Move shared top-level sources into `src/`.
4. Move simulator families into `simulators/`.
5. Move top-level documentation into `docs/`.
6. Move vendored code into `third_party/`.
7. Move helper scripts into `tools/`.
8. Rename `.travis/` to its new CI-support location and update workflows.
9. Remove clearly obsolete files.
10. Quarantine or remove large legacy structures such as
    `Visual Studio Projects/`.
11. Remove now-dead compatibility code and stale path references.

Rules:

- each step must leave the tree buildable
- no "big bang" unreviewable move of the entire repository in one commit
- each move commit should be narrowly themed and mechanically obvious

## Phase 5: Testing and Damage-Detection Plan

Because this is a clean-break layout change, testing must be stricter than
usual. The main risk is not logic regressions but broken paths, generated-file
assumptions, packaging errors, and missing assets.

### 5.1 Before Migration

Establish a baseline on the pre-move tree:

- full CMake configure/build on macOS or Linux
- full `ctest` run, recording known environmental failures separately
- representative top-level `Makefile` build smoke test
- if possible, one Windows CMake CI run and one Windows `Makefile`-style path
  validation

### 5.2 During Migration

After each phase:

- run `git grep` checks for old paths that should no longer exist
- run CMake configure from a clean out-of-tree build directory
- run a focused build of representative simulators across families
- run representative tests from:
  - PDP
  - VAX
  - IBM
  - one smaller/less common family
- verify packaging and install steps still locate runtime assets

### 5.3 Final Validation

At the end of the restructuring:

- full CMake configure/build
- full `ctest`
- `Makefile` smoke build
- verify GitHub Actions paths and script references
- verify generated CMake files are reproducible from the new layout
- verify no active build path references the old root layout
- verify the root contains only the intended small set of entrypoint files

### 5.4 Automated Checks to Add

Add new CI checks specifically for layout integrity:

- fail if new `.c` or `.h` files are added to the repository root
- fail if new simulator family directories are added outside `simulators/`
- fail if new project docs are added to the root instead of `docs/`
- fail if tracked builds write generated files into the root

## Phase 6: Documentation and Contributor Guidance

Update contributor-facing documentation to make the new layout durable:

- root `README.md` should explain the high-level tree
- add a short `docs/project/layout.md` describing ownership of:
  - `src/`
  - `simulators/`
  - `docs/`
  - `third_party/`
  - `tools/`
  - `tests/`
- update build docs and developer docs to use only the new paths
- update AGENTS guidance if needed so future automation respects the layout

## Open Follow-Up Work

These items are intentionally deferred but should be tracked:

- convert Word-format content in `docs/legacy-word/` into modern text formats
- decide whether `display/` should remain an in-tree component or be treated as
  third-party
- decide whether `Visual Studio Projects/` can be removed outright
- normalize names and casing only after the directory ownership model has
  stabilized
- revisit whether the top-level `Makefile` remains a first-class build path
  long term
