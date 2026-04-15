# Host-Side Unit Tests

SIMH now has a host-side unit-test harness under `tests/unit/`.
These tests are separate from the simulator regression suites driven by
`.ini` and `.do` files. They are intended for common host-side code such
as `src/runtime/`, extracted helpers from `src/core/`, and other code
that can be exercised directly without booting a simulated machine.

## Framework

The unit-test framework is `cmocka`.

The top-level CMake build enables host-side unit tests with the
`WITH_UNIT_TESTS` option. When `cmocka` is available, CMake creates the
`unittest` interface target, adds `tests/unit/`, and registers unit test
executables with `ctest`.

## Layout

- `tests/unit/CMakeLists.txt`
  Registers host-side unit-test executables with `add_unit_test`.
- `tests/unit/support/`
  Shared support code linked into every unit-test executable.
- `tests/unit/fixtures/`
  Golden files and other checked-in fixture data used by unit tests.
- `tests/unit/test_unit_smoke.c`
  Minimal smoke test proving the harness is working.
- `tests/unit/test_unit_support.c`
  Example test for the support helpers.

## Adding A Unit Test

1. Add a new test source file under `tests/unit/`.
2. Register it in `tests/unit/CMakeLists.txt` with `add_unit_test`.
3. Put reusable helpers in `tests/unit/support/` instead of duplicating
   them across tests.
4. Put checked-in fixture files in `tests/unit/fixtures/`.

Current examples:

```cmake
add_unit_test(unit-support
    LABEL unit
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_support.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/support
)
```

## Support Library

The shared support library is `simh_unit_support`. Its public interface
is declared in `tests/unit/support/test_support.h`.

Current helpers:

- `simh_test_source_root`
  Return the repository source root used for the current test build.
- `simh_test_binary_root`
  Return the CMake build directory for the current test build.
- `simh_test_fixture_path`
  Build a full path to a file under `tests/unit/fixtures/`.
- `simh_test_make_temp_dir`
  Create a temporary directory under the current build tree.
- `simh_test_read_file`
  Read a file into memory.
- `simh_test_read_fixture`
  Read a fixture file into memory.
- `simh_test_write_file`
  Write a file from a memory buffer.
- `simh_test_files_equal`
  Compare two files byte-for-byte.
- `simh_test_remove_path`
  Remove a file or recursively remove a directory tree.

These helpers are intentionally simple. Add to them as new unit tests
need stable infrastructure.

## Building And Running

Configure a build with unit tests enabled:

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -S . -B cmake-build-unitharness
```

Build specific unit-test executables:

```sh
ninja -C cmake-build-unitharness simbase-unit-smoke simbase-unit-support
```

Run just the host-side unit tests:

```sh
ctest --test-dir cmake-build-unitharness \
  -R 'simbase-unit-(smoke|support)' \
  --output-on-failure
```

List all tests currently known to `ctest`:

```sh
ctest --test-dir cmake-build-unitharness -N
```

## Conventions

- Keep tests readable and explicit.
- Prefer checked-in fixtures for file-format behavior.
- Prefer support-library helpers over ad hoc temp-file code.
- Characterization tests are fine when cleaning up legacy code.
- It is acceptable for test code to be larger than the production code
  it protects.

The goal is not minimal test code. The goal is clear, dependable tests
for the common host-side codebase.
