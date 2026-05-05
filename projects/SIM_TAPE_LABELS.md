# Sim Tape Labels

The `sim_tape.c` ANSI and DOS11 memory-tape paths construct fixed-width
on-tape directory and label fields.  Some immediate memory-safety issues
have been repaired, but several fields still need explicit validity
policy before formatting.

Remaining work:

- Validate ANSI file sequence numbers before writing the four-character
  `HDR1.file_sequence` field.
- Validate ANSI data block counts before writing the six-character
  `EOF1.block_count` field.
- Review other ANSI numeric label fields for the same pattern: a value
  should either fit the field or return a clear attach/build error.
- Keep tests at the generated-tape boundary where feasible, using the
  ANSI label structs in `sim_tape_internal.h` and `offsetof()` rather
  than raw byte offsets.

This is separate from dynamic string adoption.  Dynamic strings address
path and command construction truncation; this file tracks correctness
of fixed-width metadata fields that must remain fixed width on tape.
