# TODO Items

## Pending Items

### CMake

- Clean up and simplify the CMake build system.
  - Split the top-level [CMakeLists.txt](/Users/perry/proj/simh/CMakeLists.txt)
    into smaller modules so that option handling, dependency setup,
    platform quirks, feature reporting, simulator inclusion, and
    packaging are not all mixed together in one file.
  - Reduce global build state in the CMake configuration:
    replace global `include_directories`, `link_directories`, and broad
    `CMAKE_*_PATH` mutation with target-scoped include paths, link
    dependencies, and interface targets wherever practical.
  - Decide on a single source of truth for simulator build metadata.
    The current arrangement still treats the top-level `Makefile` as the
    authority and generates `cmake/simh-simulators.cmake` and many
    per-simulator `CMakeLists.txt` files from it. Either:
    - keep one manifest and generate both build systems from it, or
    - make CMake authoritative and reduce the legacy Makefile to a
      compatibility layer.
  - Refactor simulator-target declaration so the generated per-simulator
    files stay declarative without repeating avoidable boilerplate.
    Preserve explicit source lists, but reduce repeated `LABEL`,
    `PKG_FAMILY`, test wiring, and common include setup where possible.
  - Simplify the simulator core-library matrix in
    `cmake/add_simulator.cmake`. Re-evaluate whether the current
    combination of base, `INT64`, `ADDR64`, video, AIO, and BESM6
    special-case static libraries can be expressed with a clearer common
    structure, likely using more target composition and less repeated
    library setup logic.
  - Separate dependency discovery from dependency superbuild logic.
    `dep-locate.cmake` and `dep-link.cmake` currently mix package
    probing, fallback download/build decisions, and SIMH-specific link
    wiring in ways that are hard to follow.
  - Reduce package-specific branching in `dep-link.cmake` by wrapping
    each optional subsystem behind clearer setup functions or interface
    targets:
    regex, video, networking, and pcap should each have a cleaner
    boundary.
  - Remove dead code and stale commented-out logic from the CMake
    helpers, including the old simulator-regeneration block in the
    top-level `CMakeLists.txt`.
  - Clean up the platform-quirk layer so compiler and linker behavior is
    expressed in one consistent style. Minimize direct mutation of
    `CMAKE_C_FLAGS_*` and centralize feature-warning/LTO/warnings-as-
    errors handling.
  - Fix obvious correctness issues in the existing CMake helper modules,
    including:
    - variable-name typos in `platform-quirks.cmake`
    - the malformed conditional around `WARNINGS_FATAL` /
      `RELEASE_LTO`
    - the broken variable reference in `fix_interface_libs` in
      `dep-link.cmake`
    - duplicated or missing initialization of target flag variables
  - Revisit the Homebrew, vcpkg, and legacy dependency-search logic and
    reduce ad hoc path surgery where modern package targets or cleaner
    toolchain handling can replace it.
  - Revisit the recursive/superbuild dependency strategy for Ninja on
    Windows and document the intended long-term direction clearly:
    retain it as-is, narrow it, or move more users toward external
    package managers.
  - Improve CMake documentation after the structural cleanup so
    `README-CMake.md` describes the actual architecture and maintenance
    workflow rather than a historically layered one.

### Other Items

- Remove the legacy SIMH integer typedef layer where it now duplicates
  standard fixed-width C types. Migrate the codebase away from
  compatibility aliases such as `t_int64` and `t_uint64` where those are
  acting only as historical wrappers, and rely on `<stdint.h>` types
  throughout unless a type is carrying real simulator-specific
  meaning.
- Also look for other types (like `t_bool`) that could be validly
  replaced with standard types.
- Raise the project language baseline to C14 and clean out
  compatibility code that exists only for pre-C14 compilers or obsolete
  host platforms. Once that decision is made, update the build system,
  documentation, and portability assumptions consistently.
- Decide whether `src/components/display/` should remain in-tree or be
  treated as third-party code and moved under `third_party/`.
- Decide the long-term status of the top-level `Makefile`:
  continue supporting it as a first-class build path, or eventually
  retire it in favor of CMake.
- Normalize repository naming and casing policies more broadly once the
  new directory hierarchy is stable.
- Purge obsolete host operating system support references and code
  paths. Search for and remove or rewrite references to all of the
  following host platforms and closely related labels so nothing is
  missed:
  - `VMS`
  - `OpenVMS`
  - `VAX VMS`
  - `Alpha VMS`
  - `IA64 VMS`
  - `Alpha UNIX`
  - `Digital UNIX`
  - `Tru64 UNIX`
  - `Solaris`
  - `SunOS`
  - `OS/2`
  - `HP-UX`
  - `AIX`
  - `Cygwin`
  - obsolete Windows target/version names:
    `Windows 9x`, `Windows NT`, `Windows 2000`, `Windows XP`,
    `Win7`, `Win8`
  - obsolete Windows build/toolchain labels that may need review while
    host support is narrowed:
    `Visual C++`, `Visual C++ .NET`, `MinGW`, `MinGW-w64`

## Notes

- The current top-level build-output directories such as `BIN/` and
  local out-of-tree build directories remain expected generated
  artifacts rather than pending layout work.
