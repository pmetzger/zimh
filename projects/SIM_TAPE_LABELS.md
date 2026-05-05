# Sim Tape Labels

The `sim_tape.c` ANSI and DOS11 memory-tape paths construct fixed-width
on-tape directory and label fields.  Memory-safety issues in this code
have been repaired, and ANSI file sequence and block count formatting now
rejects values that do not fit the target label field.

Remaining work:

- Review other ANSI numeric label fields for the same pattern: a value
  should either fit the field or return a clear attach/build error.
- Keep tests at the generated-tape boundary where feasible, using the
  ANSI label structs in `sim_tape_internal.h` and `offsetof()` rather
  than raw byte offsets.

This is separate from dynamic string adoption.  Dynamic strings address
path and command construction truncation; this file tracks correctness
of fixed-width metadata fields that must remain fixed width on tape.
