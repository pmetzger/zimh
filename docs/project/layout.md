# Repository Layout

This repository is organized so that the top level remains small and
readable, while source ownership is explicit.

## Top-Level Ownership

- `src/`
  - Shared implementation used across multiple simulator families.
  - `src/core/` contains the simulator control program and core public
    definitions.
  - `src/runtime/` contains shared runtime libraries such as disk,
    tape, network, console, and timer support.
  - `src/components/` contains reusable in-tree components such as the
    display and frontpanel code.

- `simulators/`
  - Simulator family trees.
  - Family names and casing are preserved from the inherited project.
  - Simulator-local documentation and simulator-local tests stay with
    their family trees unless there is a later reason to centralize
    them.

- `docs/`
  - Project-level and historical documentation.
  - `docs/project/` contains current project-level material.
  - `docs/history/` contains historical notes preserved for reference.
  - `docs/legacy-word/` contains inherited Word-format material that is
    slated for later conversion.

- `third_party/`
  - Vendored or externally-derived code kept separate from in-tree
    project code.

- `tools/`
  - Build helpers, CI support, and developer utilities.
  - Root-level platform helper scripts should not be added back.

- `tests/`
  - Shared test infrastructure and shared test data.
  - Common assets that are not simulator-family-specific belong here.

## Top-Level Entrypoints

The following remain intentionally visible at the repository root:

- `README.md`
- `AGENTS.md`
- `LICENSE.txt`
- `.gitignore`
- `.gitattributes`
- `.editorconfig`
- `CMakeLists.txt`
- `Makefile`
- `cmake/`
- `vcpkg.json`

These are primary project entrypoints or policy files.

## Working Rules

- New shared `.c` and `.h` files should go under `src/`, not the root.
- New simulator families should go under `simulators/`.
- New top-level project documentation should go under `docs/`.
- New vendored code should go under `third_party/`.
- New helper scripts should go under `tools/`.
- Shared fixtures and common test data should go under `tests/`.
