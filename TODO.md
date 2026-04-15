# TODO Items

## Pending Items

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
