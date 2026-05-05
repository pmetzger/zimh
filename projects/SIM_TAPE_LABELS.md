# Sim Tape Labels

The `sim_tape.c` ANSI and DOS11 memory-tape paths construct fixed-width
on-tape directory and label fields.  Memory-safety issues in this code
have been repaired. ANSI file sequence, block count, block length, and
record length formatting now reject values that do not fit the target
label field. ANSI binary memory-tape import now validates block geometry
before padding records, so partial binary blocks fail cleanly instead of
writing past the allocated block buffer.

Remaining work:

- Review other ANSI numeric label fields for the same pattern: a value
  should either fit the field or return a clear attach/build error.
- HDR3 RMS attributes were reviewed while fixing binary block geometry.
  They currently receive either the fixed 512-byte binary record size or
  text sizes already capped below the four-hex-digit field limit. Revisit
  this if RMS sizing semantics change.
- Keep tests at the generated-tape boundary where feasible, using the
  ANSI label structs in `sim_tape_internal.h` and `offsetof()` rather
  than raw byte offsets.

This is separate from dynamic string adoption.  Dynamic strings address
path and command construction truncation; this file tracks correctness
of fixed-width metadata fields that must remain fixed width on tape.
