# SCP Save/Restore Extraction

## Problem

The simulator SAVE and RESTORE implementation lives inside
`src/core/scp.c`.  The code is mostly localized, but it is mixed into a
large command-processing file and uses several file-local constants whose
meaning is not obvious from their names.

The most important example is the save-file format marker:

```c
const char save_vercur[] = "V4.0";
const char save_ver40[] = "V4.0";
const char save_ver35[] = "V3.5";
const char save_ver32[] = "V3.2";
const char save_ver30[] = "V3.0";
```

These strings are not ZIMH release versions.  They are persistent
on-disk schema versions for simulator save files.  They tell RESTORE how
to parse the file layout.  They should be named and documented as save
format markers so nobody confuses them with `ZIMH_VERSION`.

## Current Behavior

The current save path writes the following high-level structure:

1. The save-file format marker, currently `V4.0`.
2. The simulator save name, usually the simulator name.
3. Integer size, address size, and Ethernet capability strings.
4. Simulated time and relative time.
5. A V4.0 metadata line.  After the ZIMH versioning work this is:
   `zimh version: <ZIMH_VERSION>`.
6. For each saved device:
   - device name;
   - logical name;
   - device flags;
   - unit state;
   - memory-like unit contents with zero-block compression;
   - register names, depths, and values.
7. Blank-line sentinels marking the end of each register list and the
   end of the device list.

The current restore path reads the format marker and supports `V4.0`,
`V3.5`, `V3.2`, and `V3.0` layouts.  It warns when restoring older
formats, rejects incompatible simulator names, and checks integer and
address size strings for supported newer formats.

The saved ZIMH version line is informational.  It must not become a
compatibility key unless we deliberately define a new policy for that.

## Goal

Extract SAVE and RESTORE machinery from `src/core/scp.c` into a focused
core module without changing save-file behavior.

The extraction should make the save-file format contract obvious,
document the format markers, and create a better test boundary for
future save-format changes.

## Proposed Structure

Add:

```text
src/core/scp_save.c
src/core/scp_save.h
```

Keep in `src/core/scp.c`:

- command table entries;
- `save_cmd`;
- `restore_cmd`;
- command-line parsing and switch handling if that remains cleaner near
  the command layer.

Move to `src/core/scp_save.c`:

- `sim_save`;
- `sim_rest`;
- save-file format marker constants;
- local save/restore read and write helpers;
- comments documenting the persistent format marker contract.

Expose from `src/core/scp_save.h` only the API needed outside the
module, probably:

```c
t_stat sim_save(FILE *sfile);
t_stat sim_rest(FILE *rfile);
```

If tests need direct access to the current format marker, prefer a
small query function over exposing mutable data:

```c
const char *sim_save_format_current(void);
```

Do not put the format marker in `zimh_version.h`.  The save-file format
is an on-disk schema version, not a project release version.

## Naming

Rename the format constants during extraction:

- `save_vercur` -> `sim_save_format_current_marker`
- `save_ver40` -> `sim_save_format_v40_marker`
- `save_ver35` -> `sim_save_format_v35_marker`
- `save_ver32` -> `sim_save_format_v32_marker`
- `save_ver30` -> `sim_save_format_v30_marker`

Use a comment near these constants that says:

- the marker is the persistent SAVE/RESTORE file format version;
- it is independent of `ZIMH_VERSION`;
- changing the file layout requires either preserving backward parsing
  or adding a new marker and conditional restore path;
- the marker should not change for ordinary release/version changes.

## Dependencies To Expect

The extraction will need to account for these dependencies from `scp.c`
and the shared core:

- `sim_devices` and `sim_internal_devices`;
- `sim_savename`;
- `sim_switches`, `sim_quiet`, and command switches;
- `detach_all`;
- `read_line`;
- attach/detach support;
- `DEVICE`, `UNIT`, and `REG`;
- `get_rval`;
- `scp_device_data_size_bytes`;
- simulator timing state such as `sim_vm_time` and `sim_rtime`;
- logging and diagnostics such as `sim_debug` and `sim_printf`.

If any of these are currently file-local in `scp.c`, either keep the
dependent wrapper in `scp.c` or expose a narrow helper.  Avoid broadening
global visibility more than necessary.

## Test Plan

Follow `projects/TESTING.md`: add characterization tests before moving
behavior.

Recommended tests:

1. A save-file smoke test that creates a small save file and asserts the
   first line is the current save-format marker.
2. A test that asserts the V4.0 metadata line starts with
   `zimh version: ` and includes `ZIMH_VERSION`.
3. A restore characterization test for a minimal V4.0 save file, if a
   stable minimal fixture can be created.
4. Tests that older supported markers (`V3.5`, `V3.2`, `V3.0`) still
   select the intended parse path, using fixtures if practical.
5. A negative test for an unknown format marker.
6. An integration test that saves and restores one representative
   simulator state and verifies the restored state, not just successful
   return status.

Keep fixture files small and deterministic.  If binary fixture churn is
hard to review, generate fixtures in test setup from the public save
API, then corrupt or rewrite only the marker fields needed for parser
tests.

## Migration Steps

1. Add tests around current save-format behavior while the code still
   lives in `scp.c`.
2. Rename the format marker constants in place and add prominent
   comments.
3. Add `scp_save.h` with the intended public API.
4. Move `sim_save`, `sim_rest`, and private helpers into
   `scp_save.c`.
5. Update CMake source lists for the core libraries.
6. Run focused unit tests, then the full build and full CTest pass.
7. Only after tests are green, consider additional cleanup around
   command parsing wrappers in `scp.c`.

## Non-Goals

- Do not change the save-file format as part of the extraction.
- Do not tie save compatibility to `ZIMH_VERSION`.
- Do not remove support for older save markers unless that is a
  separate, explicit compatibility decision.
- Do not rewrite device/unit/register serialization policy in the same
  change.

## Open Questions

- Should the current save-format marker be exposed only to tests, or is
  there a legitimate runtime use for reporting it?
- How much of restore switch handling belongs in `scp.c` versus
  `scp_save.c`?
- Do we want long-term text or binary fixtures for old save formats?
