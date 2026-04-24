# Versioning

ZIMH uses calendar versioning for the public project, package, and
runtime version.

## Public Version

The public version has this form:

```text
year.month.day[.patch]
```

For example:

```text
2026.4.24
2026.4.24.1
```

The first three components are the release or project snapshot date.
The optional fourth component is a same-day patch or respin number.  If
the patch value is zero, omit it from the public version string.

The public version identifies date, not API compatibility.  Document
compatibility-affecting changes in release notes.

## Source Of Truth

CMake is the source of truth for ZIMH version components.  Do not edit a
C string version by hand.

The top-level `CMakeLists.txt` defines numeric components:

```cmake
set(ZIMH_VERSION_MAJOR 2026)
set(ZIMH_VERSION_MINOR 4)
set(ZIMH_VERSION_PATCH 24)
set(ZIMH_VERSION_TWEAK 0)
```

CMake derives:

- `ZIMH_VERSION`, such as `2026.4.24`;
- `ZIMH_VERSION_DATE`, such as `20260424`.

`ZIMH_VERSION_DATE` is a packed numeric `YYYYMMDD` date.  It is derived
from the date components and must not be maintained separately.

## Generated Header

CMake configures `cmake/zimh_version.h.in` into the build directory:

```text
build/.../generated/zimh_version.h
```

Runtime C code should include `zimh_version.h` when it needs the public
version constants.

The generated header provides:

```c
#define ZIMH_VERSION_MAJOR 2026
#define ZIMH_VERSION_MINOR 4
#define ZIMH_VERSION_PATCH 24
#define ZIMH_VERSION_TWEAK 0
#define ZIMH_VERSION_DATE 20260424
#define ZIMH_VERSION "2026.4.24"
```

There is no checked-in fallback header.  CMake is the only supported
build path.

## Runtime Banner

The startup banner and `SHOW VERSION` should identify the project as
`ZIMH`, not `Open ZIMH` or `Open SIMH`.

Example:

```text
MicroVAX 3900 simulator ZIMH 2026.4.24
```

Tests should derive expected banner text from `ZIMH_VERSION` instead of
hardcoding the current date version.

## Packaging

The public package and binary name is `zimh`.

CMake and CPack package metadata should use the same derived version
components.  Avoid separate hand-written package version strings.

The package suffix option is `ZIMH_PACKAGE_SUFFIX`.  Do not add
compatibility aliases for old `SIMH_*` package version names.

## Git Provenance

Do not reintroduce build-time git commit interpolation, generated git
commit headers, or archive substitution fallback machinery.

The public version should come from the CMake version components.
Commit provenance belongs in source control and release notes, not in
runtime version constants.

## Save Files

SAVE/RESTORE format markers, such as `V4.0`, are persistent on-disk
schema versions.  They are independent of `ZIMH_VERSION`.

Do not change a save-file format marker for an ordinary release date
change.  Change or add a marker only when the save-file layout changes
and RESTORE needs conditional parsing for the new layout.

The informational `zimh version: <ZIMH_VERSION>` line in V4.0 save files
is not currently a compatibility key.

## Release Checklist

When updating the public version:

1. Update the numeric `ZIMH_VERSION_*` components in `CMakeLists.txt`.
2. Configure the build and inspect the generated `zimh_version.h`.
3. Check that CPack output names use the expected public version.
4. Run the focused version/banner tests.
5. Run the full build and test pass before release.
