# TMXR Line Count Type Cleanup

## Problem

`TMXR.lines` is declared as `int32`, but its semantic domain is a count:
the number of line descriptors owned by the multiplexer. Negative values
do not appear to be meaningful. The code treats line indexes as signed
values before validation, but the count itself should be non-negative.

This shows up during `-Wsign-compare` cleanup because tests and runtime
code often compare array indexes or size values against `mp->lines`.
Cleaning those warnings locally can hide the larger design issue unless
we track it separately.

## Important Distinction

There are two different concepts that should not share one signedness
model:

- line count: non-negative count of valid line descriptors;
- parsed line index: signed or otherwise explicitly validated value that
  may be invalid before range checking.

The long-term shape should make that distinction obvious. For example,
`TMXR.lines` could become an unsigned count type while local parsed line
numbers remain signed until after validation.

## Risks

This should not be done as a drive-by warning cleanup. `TMXR` is shared
across many simulators, and `lines == 0` is a valid unconfigured state in
some paths. A careless unsigned conversion could introduce underflow in
expressions such as `mp->lines - 1`.

Known areas to review:

- static `TMXR` initializers across simulator families;
- dynamic line-count setters in PDP-10, PDP-11, 3B2, NOVA, B5500, and
  other multiplexed devices;
- command parsing that uses `get_uint(..., mp->lines - 1, ...)`;
- checks that reject negative user-supplied line indexes;
- display code and format strings that currently use `%d`;
- tests and internal helpers that temporarily set `lines` to smaller
  fixture values.

## Proposed Approach

1. Add focused TMXR unit tests that document current behavior for:
   - `lines == 0`;
   - negative parsed line indexes;
   - line indexes equal to or greater than the line count;
   - dynamic line-count changes where existing tests already cover a
     simulator-specific setter.
2. Introduce small helper functions or validation helpers if they make
   the count/index boundary clearer.
3. Change `TMXR.lines` to a non-negative count type only after the tests
   describe the boundary behavior.
4. Update callers in small subsystem commits, keeping parsed line indexes
   signed until validation.
5. Regenerate the warnings report and confirm any `-Wsign-compare`
   changes are expected side effects of the type cleanup.

## Relationship to `-Wsign-compare`

The current `-Wsign-compare` pass may still fix local warnings in tests
or runtime code, but it should avoid making broad TMXR API type changes.
This file tracks the larger cleanup so it can be handled deliberately
later.
