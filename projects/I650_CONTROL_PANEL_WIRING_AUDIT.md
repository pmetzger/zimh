# I650 Control Panel Wiring Audit

## Problem

`simulators/I650/i650_cdp.c` decodes several IBM 650 card punch
control-word digits that are documented in comments but not implemented
as distinct behavior.  GCC reports these as `-Wunused-but-set-variable`
warnings.  Leave those warning sites in place until the underlying card
punch behavior is audited.

The warning cleanup around this file may still make harmless helpers
file-local and annotate fixed callback parameters.  It should not hide
the control-panel warnings by mechanically deleting variables or by
turning the assignments into unnamed digit consumption.  The warning is
the current signal that this may be an emulator completeness problem.

The affected paths include at least:

- SOAP/SOAPA output: control digit `a`, described as the non-blank type
  flag.
- SuperSOAP output: control digits `j`, `i`, `h`, `g`, `f`, and `b`.
- IT output: control digits `j`, `i`, `g`, `f`, `e`, and `c`; digit `g`
  is described as PIT-card selection.
- FORTRANSIT output: control digits `h`, `f`, `e`, `d`, `c`, and `b`.

## Approach

1. Gather the relevant IBM 650 SOAP, SuperSOAP, IT, and FORTRANSIT
   documentation already referenced by the source comments.
2. For each ignored digit, determine whether it represents:
   - an intentionally unsupported format,
   - a format that existing integration decks never exercise, or
   - a real missing behavior in the card punch encoder.
3. Add focused tests before any semantic repair.  If direct unit tests
   are too hard, first create a small test seam around card punch output
   encoding so expected card images and print lines can be asserted
   without running full compiler decks.
4. After behavior is understood, either implement the missing card punch
   behavior or replace genuinely ignored digit decoding with explicit
   documented consumption.
5. Keep any behavior fixes separate from mechanical compiler-warning
   cleanup commits.

## Current Safety Net

The existing I650 integration test compares generated deck output for
several punch paths, including FORTRANSIT, SuperSOAP, SOAP4, and related
compiler flows.  That is enough to guard the current warning cleanup
against accidental output changes, but it is not enough to prove that
all documented control-panel digit combinations are implemented.
