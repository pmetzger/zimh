# I650 Control Panel Wiring Audit

## Problem

The IBM 650 card reader and punch code decodes several card format and
control-panel details that are documented in comments but not fully
plumbed into distinct behavior.  GCC reports these as
`-Wunused-but-set-variable` warnings.  Leave those warning sites in
place until the underlying card behavior is audited.

The warning cleanup around these files may still make harmless helpers
file-local and annotate fixed callback parameters.  It should not hide
the card-format warnings by mechanically deleting variables or by
turning the assignments into unnamed digit consumption.  The warning is
the current signal that this may be an emulator completeness problem.

The affected punch paths in `simulators/I650/i650_cdp.c` include at
least:

- SOAP/SOAPA output: control digit `a`, described as the non-blank type
  flag.
- SuperSOAP output: control digits `j`, `i`, `h`, `g`, `f`, and `b`.
- IT output: control digits `j`, `i`, `g`, `f`, `e`, and `c`; digit `g`
  is described as PIT-card selection.
- FORTRANSIT output: control digits `h`, `f`, `e`, `d`, `c`, and `b`.

The affected reader paths in `simulators/I650/i650_cdr.c` include at
least:

- SOAP/SOAPA input: column 80 is saved as `col80` but not used.  The
  surrounding comments say column 80 contains multipass punches.
- SuperSOAP input: column 80 is saved as `col80` but not used.
- FDS input: negative overpunch state is saved in `IsNeg` while decoding
  each card column, but the value is not used outside the scan loop.

## Approach

1. Gather the relevant IBM 650 SOAP, SuperSOAP, IT, FORTRANSIT, and FDS
   documentation already referenced by the source comments.
2. For each ignored digit or card-column state, determine whether it
   represents:
   - an intentionally unsupported format,
   - a format that existing integration decks never exercise, or
   - a real missing behavior in the card reader or punch encoder.
3. Add focused tests before any semantic repair.  If direct unit tests
   are too hard, first create a small test seam around card punch output
   encoding and card reader decoding so expected card images, print
   lines, and decoded `IOSync` words can be asserted without running
   full compiler decks.
4. After behavior is understood, either implement the missing card punch
   behavior or replace genuinely ignored digit decoding with explicit
   documented consumption.
5. Keep any behavior fixes separate from mechanical compiler-warning
   cleanup commits.

## Current Safety Net

The existing I650 integration test reads input decks and compares
generated deck output for several paths, including FORTRANSIT,
SuperSOAP, SOAP4, and related compiler flows.  That is enough to guard
the current warning cleanup against accidental output changes, but it is
not enough to prove that all documented control-panel digit and card
column combinations are implemented.
