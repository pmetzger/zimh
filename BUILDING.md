# Building the Project

This file explains how to build the project from a normal developer
checkout.

The project uses CMake and supports out-of-tree builds. Do not try to
configure directly in the repository root.

The top-level `Makefile` is a thin compatibility wrapper over the
`build/release` CMake build. The native interface is still CMake and
CTest; the `make` entry points simply forward into that configured tree.

## Compiler Support and OS Support

We support builds for machines running recent vintage POSIX operating
systems and recent vintage Microsoft Windows. We support builds for
relatively recent compilers on those operating systems.

Unlike the SIMH project, we do not support building on very old
operating systems and compilers. Our goal is to have clean,
supportable code that can be maintained into the very far future, and
unfortunately, support for building on very old configurations (say
VMS) is too difficult, both because we cannot set up continuous
integration for such systems and because they complicate the code
significantly.

We currently aim at code supporting the C17 standard, but we have
checked that the code will build and run with the C standard set
anywhere from C99 to C23 successfully. We do not explicitly support
compilers from before C11, and will not take patches to fix problems
on very old toolchains.

## What you need installed

At minimum:

- a C compiler toolchain
- `cmake`
- `ninja`
- `git`
- `pkg-config`
- `bison`
- `libpcre2`

PCRE2 provides the regular expression backend used by SCP EXPECT
commands, and is required for build.

For the full feature build, install the libraries used by optional
features:

- `SDL2`
  Provides the windowing, input, and rendering layer used by the
  project’s video/graphics support, and is required for video-enabled
  builds.
- `SDL2_ttf`
  Adds text rendering for SDL-based displays and is required for
  video-enabled builds.
- `freetype`
  Supplies font loading and glyph rasterization used by the SDL display
  stack. Freetype is required for video-enabled builds.
- `libpng`
  Adds PNG image support used by graphics and image-loading paths.
- `zlib`
  Provides compression support used by image and related file-handling
  code.
- `libedit`
  Enables SCP command-line editing and history support. Unfortunately,
  libedit is not available for Windows.

For testing, you will need:

- `cmocka`
  Provides the host-side C unit-test framework used under `tests/unit/`.

For networking, you will need the libraries for the backends you enable:

- `libpcap`
  Enables direct host-interface packet capture and injection for the
  optional network backends.
- `libslirp`
  Provides the SLIRP/NAT backend when SLIRP support is enabled with the
  external libslirp backend. POSIX builds require libslirp 4.7.0 or
  newer. Windows builds require libslirp 4.9.0 or newer because older
  releases do not expose the socket-handle callbacks needed on Win64.

Notes:

- The source tree assumes a compiler with at least C11 support. A modern
  C17-capable compiler is preferred.
- `bison` is required when regenerating parser sources from the checked-in
  `.y` grammars, including the SAGE and AltairZ80 Motorola 68000 parsers.
- `cmocka` is for host-side unit tests rather than the historical
  simulator binaries themselves.
- `SDL2_ttf` is the usual missing piece when a video-enabled build does
  not configure successfully.
- `libedit` is optional, but without it SCP falls back to plainer
  console input without interactive line-editing support.
  `libedit` support is not currently available for Windows because no
  port of the library to Windows exists.
- SLIRP/NAT builds using the external backend require `libslirp`; on
  Debian and Ubuntu this is normally provided by `libslirp-dev`.
- Some networking features may also use additional system libraries,
  depending on the platform and enabled options.

## Installing dependencies

The repository already contains a helper script for installing build and
feature dependencies on common Unix-like environments:

```sh
tools/ci/deps/deps.sh linux
tools/ci/deps/deps.sh osx
tools/ci/deps/deps.sh macports
```

Use the variant that matches your environment.

If you prefer to install packages manually, make sure the compiler,
CMake, Ninja, and the feature libraries listed above are visible through
the normal compiler and `pkg-config` search paths.

## Recommended build layout

Create a dedicated build directory under `build/`, for example:

```sh
mkdir -p build/release
```

You may choose a different build directory name, but keep it separate
from the source tree.

Built executables stay inside the build tree. For the standard
layouts above, look under:

- `build/release/bin`
- `build/debug/bin`

## Full-feature build

Use this when all optional dependencies, including `SDL2_ttf`, are
installed:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B build/release
ninja -C build/release
ctest --test-dir build/release --output-on-failure --timeout 300
```

This enables the normal default feature set, including video support.

## Build without SDL/video support

If the SDL stack is incomplete, especially if `SDL2_ttf` is missing, use
this configuration instead:

```sh
cmake -G Ninja -DWITH_VIDEO=Off -DCMAKE_BUILD_TYPE=Release -S . -B build/release
ninja -C build/release -j 8
ctest --test-dir build/release -j 8 --output-on-failure --timeout 300
```

This is the safest fallback build when the graphics dependencies are not
fully installed.

## Running just the build

The compatibility `Makefile` preserves a small set of convenience entry
points. These two commands are equivalent for the default build:

```sh
make all
cmake --build build/release
```

If you have already configured successfully and only want to rebuild:

```sh
ninja -C build/release
```

The plain default build in a configured tree builds the normal default
simulator set. For example, if you use a build directory such as
`build/release`, then:

```sh
cmake --build build/release
```

builds the normal default simulator set for that configured tree.

There is also an explicit target for the experimental simulator set:

```sh
cmake --build build/release --target experimental-simulators
```

## Common build targets

Build an individual simulator by naming its target directly:

```sh
cmake --build build/release --target pdp11
cmake --build build/release --target vax
```

Common top-level targets include:

- default build
  `cmake --build build/release`
  Builds the normal default simulator set for the configured tree.
- `experimental-simulators`
  `cmake --build build/release --target experimental-simulators`
  Builds the experimental simulator set.
- `unit-tests`
  `cmake --build build/release --target unit-tests`
  Builds and runs the host-side `simh-unit-*` suite.
- `integration-tests`
  `cmake --build build/release --target integration-tests`
  Builds and runs the simulator `simh-*` suite.
- `stub`
  `cmake --build build/release --target stub`
  Builds the stub simulator skeleton under `src/components/stub/`.
  This is a developer/sample target for people working on new simulator
  integrations, not part of the normal simulator set or automated test
  suite.
- `frontpaneltest`
  `cmake --build build/release --target frontpaneltest`
  Builds the front panel sample and manual diagnostic program.
- `extra-tools`
  `cmake --build build/release --target extra-tools`
  Builds the developer/sample utility targets, currently `stub` and
  `frontpaneltest`.

Resulting executables land in the configured build tree's `bin/`
subdirectory. For example:

- `build/release/bin/pdp11`
- `build/release/bin/vax`
- `build/release/bin/frontpaneltest`
- `build/release/bin/stub`

## Running just the tests

The compatibility `Makefile` also preserves these convenience entry
points:

```sh
make unit-tests
make integration-tests
make test
```

They forward to the corresponding CMake/CTest workflows in
`build/release`.

If the build tree is already configured and built:

```sh
ctest --test-dir build/release -j 8 --output-on-failure --timeout 300
```

There are also explicit build targets for the two main test groups:

```sh
cmake --build build/release --target unit-tests
cmake --build build/release --target integration-tests
```

## Common options

Some useful CMake options:

- `-DWITH_VIDEO=Off`
  Disable SDL-based graphics support. Default: `On`.
- `-DWITH_NETWORK=Off`
  Disable optional networking features. Default: `On`.
- `-DWARNINGS_FATAL=On`
  Treat warnings as errors. Default: `Off`.
- `-DRELEASE_LTO=On`
  Enable link-time optimization in release builds. Default: `Off`.
- `-DDEBUG_WALL=On`
  Turn on stronger warning settings for debug builds. Default: `Off`.
- `-DENABLE_WINAPI_DEPRECATION_WARNINGS=On`
  Enable WinAPI deprecation warnings. Default: `Off`.
- `-DWITH_ROMS=Off`
  Disable internal ROM generation and embedding. Default: `On`.
- `-DENABLE_DEP_BUILD=On`
  Allow CMake to fetch and build supported missing dependencies into the
  local dependency prefix. This is a fallback convenience for awkward
  environments, not the preferred normal workflow. Default: `On` on
  Windows non-vcpkg builds, `Off` elsewhere.

Example:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DWITH_VIDEO=Off \
  -DDEBUG_WALL=On -S . -B build/debug
```

## Reconfiguring safely

CMake caches aggressively. If you change major options, especially
feature toggles such as `WITH_VIDEO`, it is often simplest to remove the
old build directory and configure again.

Example:

```sh
rm -rf build/release
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B build/release
```

Or use a new build directory name instead.

## Interpreting test results

In constrained sandboxed environments, one test has been known to fail
for environmental reasons rather than build correctness:

- `simh-uc15`

That simulator uses shared memory. If `shm_open` is blocked by the
environment, the test will fail even when the build itself is correct.

On a normal developer machine with shared memory available, that test
should be expected to run normally.

## Verifying dependencies manually

You can quickly check whether common dependencies are visible:

```sh
pkg-config --modversion cmocka
pkg-config --modversion sdl2
pkg-config --modversion SDL2_ttf
pkg-config --modversion freetype2
pkg-config --modversion libpng
pkg-config --modversion zlib
pkg-config --modversion libedit
pkg-config --modversion slirp
bison --version
```

If `SDL2_ttf` is missing, a full video-enabled build is not ready yet.
