# String Safety Audit

Goal: reduce use of legacy string formatting and copy APIs whose safety
depends on informal buffer-size assumptions.

This is not a blind mechanical replacement project. Some existing uses
are effectively bounded today. The purpose of the audit is to:

- identify the highest-risk cases first
- migrate to clearer bounded APIs
- improve the code's safety story without large semantic churn

## Current Inventory

Tree scan of `.c` and `.h` files found:

- `sprintf`: `1634`
- `vsprintf`: `0`
- `strcpy`: `316`
- `strcat`: `152`
- `strncpy`: `50`
- `gets`: `0`
- `strncat`: `0`

By top-level area:

- `simulators`
  - `sprintf`: `1403`
  - `vsprintf`: `0`
  - `strcpy`: `201`
  - `strcat`: `151`
  - `strncpy`: `43`

- `src`
  - `sprintf`: `231`
  - `vsprintf`: `0`
  - `strcpy`: `115`
  - `strcat`: `1`
  - `strncpy`: `7`

- `tests`
  - `sprintf`: `0`
  - `vsprintf`: `0`
  - `strcpy`: `0`
  - `strcat`: `0`
  - `strncpy`: `0`

## Priority Order

### 1. `sprintf`

`vsprintf` has already been driven to zero. The remaining largest risky
category is `sprintf`.

Desired direction:

- replace with `snprintf`
- preserve exact formatting behavior where required
- where append-style code is being used, consider restructuring around
  tracked offsets or growable strings instead of repeated formatting

### 2. `strcpy` and `strcat`

These should be reviewed together because they often appear in code that
is building strings incrementally.

Desired direction:

- use `strlcpy` / `strlcat` where fixed buffers remain appropriate
- otherwise replace with better-structured assembly logic
- prefer explicit capacity tracking over implicit assumptions

### 3. `strncpy`

These need review, not blanket replacement. Some are trying to do
bounded copies, but `strncpy` has awkward truncation and padding
semantics and is often used incorrectly.

Desired direction:

- classify each use:
  - fixed-width field copy where NUL termination is not intended
  - string copy where NUL termination is intended
- replace the second class with clearer bounded string APIs

## First Cleanup Slice

Start in `src/`, not `simulators`.

Reasons:

- smaller problem surface
- more central code
- easier to validate with unit tests
- safer place to establish replacement patterns before touching simulator
  trees

Recommended order inside `src/`:

1. obviously unsafe `sprintf` sites
2. `strcpy` / `strcat` chains that should become bounded copies
3. targeted `strncpy` review

## Likely Follow-On Work

- introduce or reuse small helper patterns for bounded append logic
- use `sim_dynstr` where dynamic assembly is genuinely cleaner than
  fixed-buffer formatting
- add focused tests whenever a migrated site has behavior worth pinning
  down

## Definition of Done

- high-risk `sprintf` sites in `src/` migrated to bounded formatting
- obvious `strcpy` / `strcat` misuse in `src/` eliminated
- a clear migration pattern established before wider `simulators/`
  cleanup
