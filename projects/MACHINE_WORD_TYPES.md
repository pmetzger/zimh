# Machine Word And Register Types

Several undefined-behavior findings have exposed a recurring type
problem: simulator code often stores uninterpreted guest machine words,
register images, device registers, and bit fields in signed host integer
types such as `int` or `int32`.

That is usually the wrong representation for values whose primary
meaning is "these bits."  Shifts, rotates, masks, byte swaps, and field
extraction should operate on unsigned types.  Signed interpretation
should happen only at the point where the simulated architecture
requires signed arithmetic, signed comparison, sign extension, or a
signed condition code.

## Audit Goal

Audit simulator and runtime code for values that are actually bit
patterns but are currently represented as signed host integers.  Convert
those storage types, helper interfaces, and nearby consumers to unsigned
types when that matches the simulated value being represented.

This should not be a mechanical `int32` to `uint32` sweep.  Each site
needs a local semantic check:

- guest machine words and register images should generally be unsigned;
- guest addresses should use address-sized unsigned types;
- host indexes, counts, lengths, and sizes should use appropriate host
  count types;
- signed guest arithmetic should keep signed interpretation local and
  explicit;
- host status codes and API results should remain status types.

## Examples Seen So Far

The VAX ROM delay path exposed a helper that was operating on a machine
word while using signed types.  The correct repair was to make the word
representation unsigned rather than merely casting inside the failing
shift.

The PDP18B/PDP9 link-plus-AC rotate path exposed a signed left shift of
a 19-bit register image.  The focused fix made the rotate operation use
an unsigned 19-bit helper, but the surrounding CPU state still deserves
a broader audit.

The ECLIPSE floating-point accumulator notes describe the same class of
problem for guest register images that are manipulated as encoded bits
but stored in signed host types.

## Process

Work one subsystem at a time, following `projects/UB_PROCESS.md`:

1. Add or improve tests before changing representation.
2. Identify the actual meaning of each value before changing its type.
3. Update helper interfaces and consumers together when they share the
   same bit-pattern domain.
4. Keep signed guest interpretation explicit and close to the operation
   that needs it.
5. Run focused sanitizer and non-sanitizer tests, then the relevant
   broader suites.

When the audit finds an area where a type change would be extensive,
create a subsystem-specific project note before doing the refactor.
