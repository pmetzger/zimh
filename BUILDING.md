# Building the Project

This file explains how to build the project from a normal developer
checkout.

The project uses CMake and supports out-of-tree builds. Do not try to
configure directly in the repository root.

## What you need installed

At minimum:

- a C compiler toolchain with at least C11 support
- `cmake`
- `ninja`
- `git`
- `pkg-config`
- `bison`

For the full feature build, install the libraries used by optional
features:

- `SDL2`
  Provides the windowing, input, and rendering layer used by the
  project’s video/graphics support.
- `SDL2_ttf`
  Adds text rendering for SDL-based displays and is required for the
  normal video-enabled build.
- `freetype`
  Supplies font loading and glyph rasterization used by the SDL display
  stack.
- `libpng`
  Adds PNG image support used by graphics and image-loading paths.
- `zlib`
  Provides compression support used by image and related file-handling
  code.
- `libedit`
  Enables SCP command-line editing and history support

For testing, you will need:

- `cmocka`
  Provides the host-side C unit-test framework used under `tests/unit/`.

For networking, you will need:

- `libpcap`
  Enables direct host-interface packet capture and injection for the
  optional network backends.

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
- Some networking features may also use additional system libraries,
  such as `libpcap`, depending on the platform and enabled options.

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

Create a dedicated build directory under `cmake/`, for example:

```sh
mkdir -p cmake-build
```

You may choose a different build directory name, but keep it separate
from the source tree.

## Full-feature build

Use this when all optional dependencies, including `SDL2_ttf`, are
installed:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B cmake-build
ninja -C cmake-build
ctest --test-dir cmake-build --output-on-failure --timeout 300
```

This enables the normal default feature set, including video support.

## Build without SDL/video support

If the SDL stack is incomplete, especially if `SDL2_ttf` is missing, use
this configuration instead:

```sh
cmake -G Ninja -DWITH_VIDEO=Off -DCMAKE_BUILD_TYPE=Release -S . -B cmake-build
ninja -C cmake-build -j 8
ctest --test-dir cmake-build -j 8 --output-on-failure --timeout 300
```

This is the safest fallback build when the graphics dependencies are not
fully installed.

## Running just the build

If you have already configured successfully and only want to rebuild:

```sh
ninja -C cmake-build
```

## Running just the tests

If the build tree is already configured and built:

```sh
ctest --test-dir cmake-build -j 8 --output-on-failure --timeout 300
```

## Common options

Some useful CMake options:

- `-DWITH_VIDEO=Off`
  Disable SDL-based graphics support.
- `-DWITH_NETWORK=Off`
  Disable optional networking features.
- `-DWARNINGS_FATAL=On`
  Treat warnings as errors.
- `-DRELEASE_LTO=On`
  Enable link-time optimization in release builds.
- `-DDEBUG_WALL=On`
  Turn on stronger warning settings for debug builds.

Example:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DWITH_VIDEO=Off \
  -DDEBUG_WALL=On -S . -B cmake-build-debug
```

## Reconfiguring safely

CMake caches aggressively. If you change major options, especially
feature toggles such as `WITH_VIDEO`, it is often simplest to remove the
old build directory and configure again.

Example:

```sh
rm -rf cmake-build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B cmake-build
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
bison --version
```

If `SDL2_ttf` is missing, a full video-enabled build is not ready yet.
