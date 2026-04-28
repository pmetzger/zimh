# Disk Test Architecture

Disk controllers throughout the tree need deterministic tests for backend
media behavior. SCSI is only one caller of `sim_disk`; per-controller hooks
will not scale well.

## Current Need

Controller tests should be able to force disk outcomes that are difficult or
non-portable to create with real host files:

- read and write I/O errors
- short reads and writes
- write-protect behavior
- media not present or media-change states
- flush/synchronize-cache behavior
- attach and detach edge cases

The tests should still use real image-backed `sim_disk` paths for normal
integration behavior. Fault injection is mainly for failures that host
filesystems hide or make unreliable.

## Implemented

`sim_disk` now has a small process-local test backend override for sector read
and write operations. The entry points are always compiled, but their comments
make clear that this is a test seam, not a stable storage plugin API. Tests
must set up and clear overrides explicitly.

Controllers should continue to call the normal `sim_disk_*` APIs. Tests should
configure the disk unit or test backend to produce the desired result, rather
than adding private hooks to each controller.

The shared backend hook is directly covered by `sim_disk` unit tests. The SCSI
unit tests also use it to force failed and short read/write transfers, and SCSI
now reports those failures with CHECK CONDITION and appropriate sense data.

## Remaining Direction

Avoid linker wrapping, weak-symbol tricks, or host-specific filesystem faults.
Those approaches are fragile across POSIX, Windows, and CMake target layouts.

When non-SCSI disk controllers need deterministic media fault tests, they
should use the shared `sim_disk` test backend rather than adding private
controller hooks.

The current backend override handles sector read and write operations. Future
tests may need additional seams for write-protect behavior, media presence or
media-change state, flush/synchronize-cache behavior, and attach/detach edge
cases.

## Acceptance Criteria

- The production `sim_disk_*` API remains the normal controller boundary.
- Unit tests can force read/write errors and short I/O deterministically.
- The shared test backend is directly covered by `sim_disk` unit tests.
- SCSI uses the shared backend hook instead of a private controller hook.
- Real image-backed tests still cover successful read/write behavior.
- At least one non-SCSI disk controller should eventually use the same seam
  when it needs deterministic media fault tests.
