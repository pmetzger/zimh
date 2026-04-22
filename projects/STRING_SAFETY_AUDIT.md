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

- `sprintf`: `1692`
- `vsprintf`: `6`
- `strcpy`: `345`
- `strcat`: `158`
- `strncpy`: `52`
- `gets`: `0`
- `strncat`: `0`

By top-level area:

- `simulators`
  - `sprintf`: `1403`
  - `strcpy`: `201`
  - `strcat`: `151`
  - `strncpy`: `43`

- `src`
  - `sprintf`: `287`
  - `vsprintf`: `6`
  - `strcpy`: `140`
  - `strcat`: `7`
  - `strncpy`: `7`

## Priority Order

### 1. `vsprintf`

These are the clearest high-priority fixes because they are inherently
unbounded formatting interfaces.

Desired direction:

- replace with `vsnprintf`
- carry buffer sizes explicitly
- review any helper APIs that currently hide the destination size

### 2. `sprintf`

Not every `sprintf` is automatically dangerous, but the count is high
enough that it should be treated as the main migration category.

Desired direction:

- replace with `snprintf`
- preserve exact formatting behavior where required
- where append-style code is being used, consider restructuring around
  tracked offsets or growable strings instead of repeated formatting

### 3. `strcpy` and `strcat`

These should be reviewed together because they often appear in code that
is building strings incrementally.

Desired direction:

- use `strlcpy` / `strlcat` where fixed buffers remain appropriate
- otherwise replace with better-structured assembly logic
- prefer explicit capacity tracking over implicit assumptions

### 4. `strncpy`

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

1. `vsprintf` sites
2. obviously unsafe `sprintf` sites
3. `strcpy` / `strcat` chains that should become bounded copies

## Likely Follow-On Work

- introduce or reuse small helper patterns for bounded append logic
- use `sim_dynstr` where dynamic assembly is genuinely cleaner than
  fixed-buffer formatting
- add focused tests whenever a migrated site has behavior worth pinning
  down

## Definition of Done

- no remaining `vsprintf` in maintained code
- high-risk `sprintf` sites in `src/` migrated to bounded formatting
- obvious `strcpy` / `strcat` misuse in `src/` eliminated
- a clear migration pattern established before wider `simulators/`
  cleanup
