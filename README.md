# RustyIron Historical Computer Simulator

RustyIron is a software emulator for important classic computer
systems and peripherals. It provides simulators, device models, and
the supporting infrastructure needed to run historical software and
operating systems with high fidelity on modern machines.

RustyIron is a hard fork of SIMH, the simulator created by Bob Supnik
and extended over many years by the SIMH team and broader contributor
community. We are grateful to Bob and the SIMH contributors for all of
their work over the years. Without their effort, none of this would be
possible.

The goals of this fork are substantially different from those of the
original project. In particular, this project is intended to pursue:

- Clean and extensive modernization of the codebase;
- Progressive conversion of the implementation to Rust;
- Uplift of the simulators into a machine-readable architecture
  specification language that can be automatically lowered into
  compiled software;
- Conversion of the documentation into text-based, non-Microsoft
  Word-based formats;
- Addition of an extensive automated test suite; and
- A much higher velocity of change than has historically been
  appropriate for SIMH itself.

Conversations with the upstream team indicated that they would almost
certainly not be interested in adopting the kinds of changes planned
here. Accordingly, this fork is being developed independently.

This project is under the MIT license; see [LICENSE.txt](LICENSE.txt)
for the specific wording.

A detailed listing of features in the inherited code base as of 12 May
2022 may be found in [SIMH-V4-status](docs/project/SIMH-V4-status.md).

## Repository Layout

The repository contains:

- `src/`: shared core, runtime, and reusable in-tree components
- `simulators/`: source trees for specific machine simulators
- `docs/`: project-level, historical, and legacy-format documentation
- `third_party/`: vendored or externally-derived code
- `tools/`: build helpers, CI support, and developer utilities
- `tests/`: shared test infrastructure and test data

Build entrypoints visible at the top level:

- `CMakeLists.txt`
- `Makefile`
- `cmake/`
- `vcpkg.json`

More detail is in [docs/project/layout.md](docs/project/layout.md).

## PLEASE NOTE

**Do not** contribute material taken from `github.com/simh/simh`
unless you are the author of the material in question; that repository
is not open source.
