# Windows Build Cleanup

This file records the Windows/MSVC build changes currently on the
`windows-build-cleanup` branch and why they were made.

## Build configuration and dependency discovery

- `CMakeLists.txt`
  Hide `WITH_VDE` on Windows and force it off there.
  Purpose: upstream VDE does not support Windows, so the Windows build
  should not probe or expose that option.

- `CMakeLists.txt`
  Change `cmocka` discovery so Windows prefers
  `find_package(cmocka CONFIG)` while non-Windows keeps the old
  `pkg-config` path.
  Purpose: `vcpkg` provides a usable CMake package on Windows, and this
  avoids `pkg-config` path breakage without changing known POSIX
  behavior.

- `cmake/dep-link.cmake`
  Link the actual PCRE2 library when `PCRE2_FOUND` is true.
  Purpose: the Windows build was finding PCRE2 headers but not linking
  the library, which caused unresolved symbols.

- `third_party/slirp/CMakeLists.txt`
  Add `src/compat` to the private include path for `slirp`.
  Purpose: `sim_slirp.c` needs compatibility headers during the Windows
  build.

- `vcpkg.json`
  Add `builtin-baseline` and remove the `pkgconf` dependency.
  Purpose: the manifest needs a baseline for deterministic resolution,
  and `pkgconf` is no longer needed for the normal Windows build path.

## Parser tool handling

- `simulators/AltairZ80/CMakeLists.txt`
- `simulators/SAGE/CMakeLists.txt`
  Accept either `bison` or `win_bison`.
  Purpose: common Windows installs provide `win_bison.exe` rather than
  `bison.exe`.

- `projects/PARSER_GENERATION_POLICY.md`
  Document the parser-generation requirement and the `win_bison`
  executable name.
  Purpose: record the policy and the Windows-specific tool name issue.

## Runtime and compatibility code

- `src/runtime/sim_time.c`
  Add the `sim_clock_gettime()` wrapper implementation for Windows using
  `GetSystemTimePreciseAsFileTime()`.
  Purpose: provide a modern Windows wall-clock implementation and make
  time reads testable.

- `src/runtime/sim_timer.c`
  Change `sim_timenow_double()` to use `sim_clock_gettime()`, and use
  `GetSystemTimePreciseAsFileTime()` in the local Windows fallback.
  Purpose: remove the last direct `clock_gettime()` dependency from the
  Windows build and keep all wall-clock reads on one path.

- `src/runtime/sim_frontpanel.c`
- `third_party/slirp_glue/glib_qemu_stubs.c`
  Switch Windows wall-clock code from `GetSystemTimeAsFileTime()` to
  `GetSystemTimePreciseAsFileTime()`.
  Purpose: use the modern precise Windows API consistently.

- `src/runtime/sim_time_win32.h`
  Remove the fallback `struct timespec` definition and leave only the
  `CLOCK_REALTIME` constant and `clock_gettime()` declaration.
  Purpose: assume a modern Windows CRT and stop carrying older-header
  compatibility logic.

- `src/compat/setenv.c`
- `src/compat/sim_compat.h`
- `cmake/add_simulator.cmake`
- `src/core/scp.c`
  Move the Windows `setenv`/`unsetenv` shim out of `scp.c` into
  `src/compat`, declare it in `sim_compat.h`, and compile it only on
  Windows.
  Purpose: make the shim a proper compatibility routine rather than a
  private SCP implementation, and fix link failures from other objects
  that use it.

- `src/compat/README.md`
  Add `setenv` and `unsetenv` to the documented scope of `src/compat`.
  Purpose: keep the compatibility layer documentation accurate.

## Tests

- `tests/unit/src/compat/test_compat_env.c`
- `tests/unit/src/compat/CMakeLists.txt`
  Add a host-side unit test for the Windows `setenv`/`unsetenv` shim.
  Purpose: verify create, overwrite, unset, and invalid-input behavior.

## Documentation

- `BUILDING.md`
  Update Windows build notes:
  remove `pkg-config` as a universal prerequisite,
  document `win_bison`,
  note that VDE is unavailable on Windows,
  and clarify that POSIX hosts rely on `pkg-config`.
  Purpose: make the build documentation match the actual supported
  Windows path.

## Temporary diagnostics in `tmp/`

- `tmp/capture-stub-build.cmd`
- `tmp/capture-stub-ninja-explain.cmd`
- `tmp/capture-stub-ninja-dryrun.cmd`
- `tmp/run-first-stub-step.cmd`
- `tmp/capture-unit-compat-env.cmd`
- `tmp/capture-unit-compat-env-dryrun.cmd`
- `tmp/capture-unit-support-build.cmd`
- `tmp/run-compat-env-standalone.cmd`
- `tmp/compile-windows-cleanup-sources.cmd`
  Temporary helper scripts used to capture visible MSVC/Ninja output and
  verify the Windows fixes when the normal build wrapper was opaque.
  Purpose: debugging and validation during the Windows bring-up work.
