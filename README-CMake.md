# Building with CMake

This project uses CMake as the source of truth for build metadata.
Simulator-local `CMakeLists.txt` files, the top-level simulator
inventory, and the packaging metadata are all maintained directly in
CMake.

The top-level `Makefile` is only a thin compatibility wrapper over the
default `build/release` CMake build. The native interface is CMake and
CTest.

For the normal build instructions, start with:

- `BUILDING.md`

For build-system maintenance guidance, see:

- `cmake/CMake-Maintainers.md`
- `cmake/CMake-Simulator-Maintainers.md`

## Preferred Build Layout

Use out-of-tree builds under `build/`:

- `build/release`
- `build/debug`

Typical release build:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B build/release
cmake --build build/release
ctest --test-dir build/release --output-on-failure
```

Typical debug build:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -S . -B build/debug
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

## Common Targets

Build the default simulator set:

```sh
cmake --build build/release
```

Build one simulator:

```sh
cmake --build build/release --target pdp11
cmake --build build/release --target vax
```

Build the experimental simulator set:

```sh
cmake --build build/release --target experimental-simulators
```

Build and run the host-side unit tests:

```sh
cmake --build build/release --target unit-tests
```

Build and run the simulator integration tests:

```sh
cmake --build build/release --target integration-tests
```

Build the developer/sample utility targets:

```sh
cmake --build build/release --target extra-tools
cmake --build build/release --target stub
cmake --build build/release --target frontpaneltest
```

## Useful CMake Options

Project-defined options follow two patterns:

- `WITH_*` for runtime or product features
- `ENABLE_*` for tooling or build-time behavior

The most commonly used options are:

- `CMAKE_BUILD_TYPE`
  Default: generator-dependent.
  Use `Release` or `Debug` for single-config generators such as Ninja.
- `WITH_VIDEO`
  Default: `On`.
  Enable SDL-based graphics and display support.
- `WITH_NETWORK`
  Default: `On`.
  Enable simulator networking support.
- `WITH_REGEX`
  Default: `On`.
  Enable PCRE-based SCP regular expression support.
- `WITH_PCRE2`
  Default: `Off`.
  Prefer the experimental PCRE2 regex backend.
- `WITH_ROMS`
  Default: `On`.
  Build and embed internal ROM support where applicable.
- `ENABLE_DEP_BUILD`
  Default: platform-dependent.
  Allow supported missing dependencies to be fetched and built locally.
- `ENABLE_CPPCHECK`
  Default: `Off`.
  Enable `cppcheck` integration.
- `ENABLE_WINAPI_DEPRECATION_WARNINGS`
  Default: `Off`.
  Enable WinAPI deprecation warnings.
- `RELEASE_LTO`
  Default: `Off`.
  Enable release-mode link-time optimization.
- `DEBUG_WALL`
  Default: `Off`.
  Turn on stronger warning settings for debug builds.
- `DEBUG_WARNINGS`
  Default: `Off`.
  Add stricter debug-only warning flags; implies `-Wall` on GCC/Clang.
- `WARNINGS_FATAL`
  Default: `Off`.
  Treat warnings as errors.

Examples:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DWITH_VIDEO=Off \
  -DDEBUG_WALL=On -S . -B build/debug
```

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_DEP_BUILD=On \
  -S . -B build/release
```

`ENABLE_DEP_BUILD` is a fallback convenience for awkward environments.
It is not the preferred normal workflow on platforms where system
packages are easy to install.

## Generators

Ninja is the recommended general-purpose generator:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B build/release
```

Visual Studio generators are also supported on Windows. Example:

```powershell
cmake -G "Visual Studio 17 2022" -A Win32 -S . -B build/release
cmake --build build/release --config Release
ctest --test-dir build/release --build-config Release \
  --output-on-failure
```

## Notes for Maintainers

If you are changing how simulators are declared or packaged, use these
documents rather than extending this file:

- `cmake/CMake-Maintainers.md`
- `cmake/CMake-Simulator-Maintainers.md`

In particular:

- simulator-local metadata lives in simulator-local `CMakeLists.txt`
- `cmake/simh-simulators.cmake` owns the top-level simulator inventory
- `cmake/simh-packaging.cmake` owns packaging-family metadata
- `cmake/add_simulator.cmake` owns shared simulator build behavior

## Historical Note

Older versions of this repository used generated simulator CMake files
and generator-era helper workflows. Those are gone. Treat the CMake
files in this tree as maintained source files.
