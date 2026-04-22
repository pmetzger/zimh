# ZIMH Historical Computer Simulator

ZIMH (which stands for "ZIMH Implements Machine History"), is a
software emulator for important computer systems and peripherals from
the history of computing. It provides a mechanism by which modern
users can run and experience what it was like to use old computers
that have not been made in many decades, or use software that was
intended for systems that no longer exist.

(It is of course possible to also use the ZIMH framework to simulate
more recent or even current machines, though this has not been the
typical use case.)

ZIMH is a hard fork of SIMH, a simulator created by Bob Supnik and
extended over many years by the SIMH team. We are grateful to Bob and
the SIMH contributors for all of their work over the years. Without
their efforts, none of this would be possible.

The development track of ZIMH is substantially different from that of
the original SIMH project. In particular, this project is pursuing:

- A much higher velocity of development
- An extensive, automated, in-tree test suite sufficient to allow for
  that high velocity development
- Extensive modernization and cleanup of the codebase
- Progressive conversion of the implementation to Rust
- Uplift of the simulators into a machine-readable architecture
  specification language that can be automatically lowered into
  executable code

ZIMH has already accomplished the following goal:
- Conversion of the documentation into text-based, non-Microsoft
  Word-based formats

Conversations with the upstream team indicated that they would almost
certainly not be interested in adopting the kinds of changes planned
here. Accordingly, this fork is being developed independently and we
have changed the name of the project to ZIMH.

This project is made available under an X11 style license; see
[LICENSE.txt](LICENSE.txt) for details.

A detailed listing of features in the inherited code base as of 12 May
2022 may be found in [SIMH-V4-status](docs/history/SIMH-V4-status.md);
however, it is likely that the code will not resemble SIMH much in the
near future.

For a current catalog of the simulator targets in this tree, see
[docs/manual/simulators.md](docs/manual/simulators.md).

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
