# Eclipse FPU Register Representation

## Problem

`simulators/NOVA/eclipse_cpu.c` stores the Eclipse floating-point
accumulators in:

```c
t_int64 FPAC[4];
```

These accumulators are guest 64-bit register images. Much of the code
treats them as bit patterns: masking sign, exponent, and fraction fields;
testing the high bit; shifting fields into and out of memory; and
comparing exact encoded values.

That register-image use is naturally unsigned, but the storage type is
signed. The current `-Wsign-compare` warning in the FFMD path is a
symptom of that larger representation mismatch, not just a local typo.

## Why Not Silence The Warning Locally

A local cast at the comparison site would preserve the current C
behavior, because the high-bit constant already forces an unsigned
comparison. However, that would hide the architectural issue: the file
mixes unsigned register-image operations with signed arithmetic helper
state.

The warning should remain until we either fix the representation or make
a deliberate, tested decision about where signed and unsigned domains
meet.

## Proposed Direction

Separate register storage from arithmetic state:

- store `FPAC` as an unsigned 64-bit register image, likely `t_uint64`;
- use unsigned extraction helpers for sign, exponent, and fraction
  fields;
- keep signed arithmetic temporaries only where the value is genuinely
  arithmetic rather than an encoded register image;
- update `get_sf`, `store_sf`, `get_lf`, and `store_lf` so their types
  document whether they consume or produce register images;
- audit all shifts involving the sign bit so they do not rely on signed
  right-shift behavior;
- add focused behavior tests before changing the representation.

## Testing Strategy

This should not be done as a mechanical warning cleanup. Before changing
the representation, add tests that exercise:

- loading and storing short and long floating-point values;
- sign, zero, exponent, and fraction extraction;
- FFMD cases around the existing special bit patterns;
- arithmetic cases that currently depend on sign or overflow handling;
- the existing Eclipse integration test.

If direct FPU tests are awkward with the current simulator structure,
this is a good early candidate for the generalized in-process simulator
harness described in
`projects/SIMULATOR_IN_PROCESS_TEST_HARNESS.md`.

## Non-Goals

- Do not change NOVA/Eclipse FPU behavior while merely sweeping compiler
  warnings.
- Do not replace signed arithmetic fields with unsigned types unless the
  value is actually a register image or bit field.
- Do not add isolated casts just to hide the warning without addressing
  the representation boundary.
