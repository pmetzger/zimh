# Obsolete Platform Macros To Purge

This file tracks the preprocessor platform macros that are now considered
obsolete host-platform support in this tree and are intended to be
removed from the source code.

This list is meant to be the canonical reference for future cleanup work
such as `unifdef` passes, manual source simplification, and portability
audits.

The baseline assumption behind this file is that the project now targets
modern machines only and no longer supports VMS, Alpha hosts, Itanium
hosts, or similarly old host environments.

## Inventory source

The inventory for this file comes from the repo-local script:

- [find_ifdefs.sh](/Users/perry/proj/simh/tools/find_ifdefs.sh:1)

That script scans `src/` and `simulators/` for active conditional
directives and extracts the macro names used in them. This file tracks
the obsolete-host subset of those results.

## Canonical purge macros

The following macro spellings were found in active conditionals across
`src/` and `simulators/` and should be treated as directly purgeable
obsolete-host markers:

- `VMS`
- `_VMS`
- `__VMS`
- `__ALPHA`
- `__ia64`
- `__sun`
- `__sun__`
- `__hpux`
- `_AIX`
- `__CYGWIN__`
- `__VAX`
- `__OS2__`
- `__EMX__`
- `__ZAURUS__`
- `__HAIKU__`
- `macintosh`

## Coupled qualifier macros

The script also found this macro in obsolete-host branches, but it
should not be removed blindly everywhere:

- `__unix__`
  - remove when it is part of the coupled obsolete Alpha UNIX branch
    `defined(__ALPHA) && defined(__unix__)`
  - do not treat plain generic `__unix__` conditionals as obsolete by
    default

## Intended host-platform meaning

These macros correspond to support we intend to purge for obsolete host
platforms, including:

- VMS / OpenVMS
- Alpha UNIX / Tru64 / Digital UNIX
- Itanium / IA-64 host distinctions
- Solaris / SunOS
- HP-UX
- AIX
- Cygwin
- VAX-host-specific compatibility cases
- OS/2 / EMX
- Sharp Zaurus host-specific compatibility cases
- Haiku host-specific compatibility cases
- Classic Macintosh compiler compatibility cases

## Important notes

- This list is about obsolete **host platform** conditionals found in
  active `#if` / `#ifdef` logic under `src/` and `simulators/`.
- It does **not** automatically include every portability macro in the
  tree.
- It also does **not** treat every textual mention of these macros in
  comments, docs, or generated files as part of the purge set.
- In particular, current-platform branches such as these are not part of
  this purge list:
  - `_WIN32`
  - `__APPLE__`
  - `__linux`
  - `__linux__`
  - `__FreeBSD__`
  - `__NetBSD__`
  - `__OpenBSD__`
- This list also does **not** yet decide anything about large-file
  support conditionals such as `DONT_DO_LARGEFILE`; those are being
  handled separately.

## Usage guidance

When cleaning a file:

1. remove branches keyed on the canonical purge macros above
2. remove `__unix__` only when it is part of the coupled obsolete Alpha
   UNIX branch described above
3. simplify the remaining modern-platform logic afterward
4. do not mix this with unrelated behavioral changes unless necessary
5. search for nearby platform-specific comments, help text, and similar
   explanatory text that may now be obsolete
6. remove comments that only described deleted obsolete-platform
   branches, and rewrite any retained comments so they describe the new
   unconditional modern behavior
7. do not treat intentionally historical material such as
   `docs/history/` as a cleanup target unless that is the explicit task
8. update this file if the obsolete macro set itself changes
