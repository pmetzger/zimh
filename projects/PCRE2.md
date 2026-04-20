# PCRE2 Conversion

This project tracks the work needed to move SIMH's regular expression
support from the deprecated PCRE 1 API to PCRE2.

## Current State

The build system can already locate and link either PCRE or PCRE2, but
the runtime code is still written against the PCRE 1 API.

The main affected code paths are:

- `src/core/sim_defs.h`
  - only includes `pcre.h`
  - only defines `USE_REGEX` from `HAVE_PCRE_H`
- `src/core/scp_expect.c`
  - uses `pcre_compile`
  - uses `pcre_fullinfo`
  - uses `pcre_exec`
  - uses `pcre_free`
- `src/core/scp.c`
  - help text describes PCRE-only support
  - build reporting uses `pcre_version()`

So today:

- PCRE-backed builds provide the working EXPECT regex implementation
- PCRE2-backed builds are only partial plumbing
- the PCRE2 path should be treated as incomplete until the source port is
  finished

## What Needs To Be Done

### 1. Add a real PCRE2 runtime path

The core regex support needs a PCRE2-native implementation.

That means:

- include `pcre2.h`
- define `PCRE2_CODE_UNIT_WIDTH 8` before including it
- introduce a `USE_REGEX` path for `HAVE_PCRE2_H`
- add any compatibility wrappers or helper functions needed so the SCP
  code can compile cleanly against either backend during the transition

### 2. Port the EXPECT engine in `scp_expect.c`

The EXPECT implementation currently stores compiled regex state as PCRE 1
objects and calls PCRE 1 functions directly.

The port needs to replace that with PCRE2 equivalents, including:

- compile-time objects
  - `pcre *` -> `pcre2_code *`
- compile calls
  - `pcre_compile` -> `pcre2_compile`
- capture-group count queries
  - `pcre_fullinfo(..., PCRE_INFO_CAPTURECOUNT, ...)`
  - -> `pcre2_pattern_info(..., PCRE2_INFO_CAPTURECOUNT, ...)`
- match execution
  - `pcre_exec` -> `pcre2_match`
- match working state
  - allocate and free `pcre2_match_data`
- object cleanup
  - `pcre_free` -> `pcre2_code_free`

This is the main functional conversion.

### 3. Update `sim_defs.h`

`sim_defs.h` currently treats regex support as synonymous with
`HAVE_PCRE_H`.

That needs to become:

- a shared `USE_REGEX` enable for either backend
- backend-specific includes and helper macros where needed

This should be done carefully so the rest of the tree can test
`USE_REGEX` for feature presence without caring which backend was chosen.

### 4. Update SCP help and build reporting

The user-visible text currently says "PCRE" in several places.

That needs to be updated so:

- help text describes PCRE-compatible regular expression support without
  hard-coding the old library
- the build report shows the active backend accurately
  - PCRE
  - PCRE2
  - or no regex support

### 5. Validate both backend configurations during the transition

Before removing the PCRE path, both configurations should be tested:

- `WITH_REGEX=On`, `WITH_PCRE2=Off`
- `WITH_REGEX=On`, `WITH_PCRE2=On`

At minimum, the test pass should confirm:

- CMake configures correctly
- the tree builds
- EXPECT regex commands work
- case-insensitive regex mode still works
- capture groups still populate `_EXPECT_MATCH_GROUP_n`

### 6. Remove the old PCRE 1 path

Once PCRE2 is known-good:

- delete the PCRE 1 API code path
- delete the PCRE 1 dependency lookup/linking path
- drop the `WITH_PCRE2` selection knob
- make PCRE2 the only supported regex backend

At that point the build should simply mean:

- `WITH_REGEX=On` -> use PCRE2
- `WITH_REGEX=Off` -> no regex support

## Suggested Order

1. Add the missing `HAVE_PCRE2_H` / `USE_REGEX` support scaffolding
2. Port `scp_expect.c`
3. Update `scp.c` help text and build reporting
4. Test both backend configurations
5. Remove the old PCRE path and simplify the build logic

## End State

The desired end state is:

- PCRE2 is the only regex backend
- EXPECT regex support works exactly as it does today
- the build no longer carries a deprecated-PCRE fallback path
