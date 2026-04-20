# PCRE2 Conversion

This project tracks the remaining work needed to move ZIMH's regular
expression support from the deprecated PCRE 1 API to PCRE2.

The goal is not long-term backend neutrality. The end state should be
simple:

- `WITH_REGEX=On` means PCRE2-backed regular expression support
- `WITH_REGEX=Off` means no regular expression support
- the tree no longer contains any PCRE 1 runtime path or build option

## Current State

The build system can already locate and link either PCRE or PCRE2, but
the runtime code is still fundamentally written against the PCRE 1 API.

The main affected code paths are:

- `src/core/sim_defs.h`
  - only includes `pcre.h`
  - only defines `USE_REGEX` from `HAVE_PCRE_H`
  - stores compiled regex state in `EXPTAB` as `pcre *`
- `src/core/scp_expect.c`
  - uses `pcre_compile`
  - uses `pcre_fullinfo`
  - uses `pcre_exec`
  - uses `pcre_free`
- `src/core/scp.c`
  - help text describes PCRE-only support
  - SCP startup publishes `SIM_REGEX_TYPE=PCRE`
  - build reporting uses `pcre_version()`

So today:

- PCRE-backed builds provide the working EXPECT regex implementation
- PCRE2-backed builds are only partial plumbing
- the PCRE2 path should be treated as incomplete until the source port is
  finished

## Existing Test Coverage

There is already useful EXPECT coverage in
`tests/unit/src/core/test_scp_expect.c`.

What is already covered:

- exact-string EXPECT matching
- duplicate persistent-rule rejection
- explicit clear and clear-all paths
- one regex happy-path capture-group case under `USE_REGEX`
- SHOW rendering of pending SEND and EXPECT state

This means the PCRE2 work is not starting from zero. The missing pieces
are mostly error-path, mode-variation, and reporting coverage.

## Unit Tests Still Needed

Before or during the conversion, add the following tests.

### 1. Case-insensitive regex matching

There is currently no unit test for `EXP_TYP_REGEX_I`.

Add a test that proves:

- a regex rule compiled with `EXP_TYP_REGEX_I` matches input that differs
  in case
- the matched text and capture groups are still populated correctly

This matters because PCRE2 uses different flag names and matching
structures, so a quiet regression here would be easy.

### 2. Invalid regex syntax rejection

`sim_exp_set()` has a distinct invalid-regex path in `scp_expect.c`.

Add a unit test that proves:

- malformed regex syntax is rejected
- the return status is `SCPE_ARG`
- the rule is not installed in the expect table

This should be tested explicitly because the compile-error reporting path
will change when `pcre_compile()` becomes `pcre2_compile()`.

### 3. No-regex-support behavior

There is a `!USE_REGEX` path in `sim_exp_set()` that currently reports
"RegEx support not available".

Add a unit test that is compiled and run when `WITH_REGEX=Off` and proves:

- regex rules are rejected cleanly
- non-regex EXPECT rules still work

This keeps the no-regex build mode alive while the conversion is in
flight.

### 4. Capture-group cleanup across successive matches

The current test proves one successful regex match and one set of
capture-group environment variables.

It does not prove that old `_EXPECT_MATCH_GROUP_n` values are cleared
correctly when a later match has fewer groups.

Add a test that:

- first matches a regex with more capture groups
- then matches one with fewer
- verifies the no-longer-valid `_EXPECT_MATCH_GROUP_n` variables are
  cleared or blanked as intended

This is especially important because the PCRE2 match-data model differs
from PCRE 1's ovector handling.

### 5. Regex reporting and backend-name tests

There is currently no unit-test coverage for the regex-specific output in
`scp.c`.

Add tests that verify:

- build/version reporting prints the expected regex support line
- SCP startup publishes the expected `SIM_REGEX_TYPE` value
- no-regex builds report the absence of regex support cleanly

These tests should not rely on the whole SCP startup path or the full
`show_version()` output blob if that can be avoided.

### 6. PCRE2 configuration leg in test automation

This is not a cmocka unit test, but it is still required coverage.

The tree should be exercised in at least these build modes while the
conversion is underway:

- `WITH_REGEX=Off`
- `WITH_REGEX=On` with the current PCRE backend
- `WITH_REGEX=On -DWITH_PCRE2=On` during the transition

Once the PCRE1 path is removed, this simplifies to:

- `WITH_REGEX=Off`
- `WITH_REGEX=On`

## Refactoring Needed in `scp.c`

The regex-specific logic in `scp.c` is currently embedded directly in
large SCP code paths:

- startup hard-codes `SIM_REGEX_TYPE=PCRE`
- `show_version()` hard-codes the regex support line and backend version
- help text hard-codes PCRE wording

That makes the code harder to test than it needs to be.

The useful refactoring here is not a generic backend-neutral framework.
It is simply pulling the regex-specific reporting and environment logic
into small helper functions so they can be tested directly.

Recommended helpers:

- `const char *sim_regex_backend_name(void);`
- `const char *sim_regex_backend_version(void);`
- `void sim_publish_regex_environment(void);`
- `void sim_fprint_regex_support(FILE *st);`

Then:

- SCP startup can call `sim_publish_regex_environment()`
- `show_version()` can call `sim_fprint_regex_support(st)`

This gives unit tests direct seams for:

- backend-name reporting
- backend-version reporting
- no-regex behavior
- `SIM_REGEX_TYPE` publishing

without needing to drive all of SCP startup or inspect the full build
report output.

## Refactoring Needed in `scp_expect.c`

The main conversion work is still in `scp_expect.c`.

The important structural goal is not "support two backends forever". It
is "make the PCRE2 port less invasive and easier to verify".

Useful preparatory refactorings:

### 1. Isolate regex compile/inspect/match/free operations

Today the PCRE 1 API calls are scattered inline through:

- rule validation in `sim_exp_set()`
- rule installation in `sim_exp_set()`
- cleanup in `sim_exp_clr_tab()` and `sim_exp_clrall()`
- runtime matching in `sim_exp_check()`

Before the full port, it would help to isolate these responsibilities
into small internal helpers such as:

- compile one regex rule
- query capture-group count
- free compiled regex state
- run one regex match and expose capture offsets

These helpers do not need to be permanent abstractions. They just reduce
the amount of duplicated PCRE-specific code that has to be touched during
the conversion.

### 2. Separate regex-match result handling from backend calls

`sim_exp_check()` currently combines:

- buffer preparation
- regex library invocation
- capture-group environment export
- rule-trigger side effects

Split that into smaller internal steps so the PCRE2-specific match-data
handling is isolated from the generic "EXPECT rule matched" behavior.

This will make it easier to verify:

- the PCRE2 match-data allocation and free path
- capture-group extraction
- stale-group cleanup
- normal rule-trigger behavior after a match

### 3. Move backend-specific type decisions out of `sim_defs.h`

Right now `EXPTAB` directly stores a `pcre *`.

Before or during the port, make that representation PCRE2-based instead
of trying to preserve a long-lived dual-backend type layer. The point is
to reduce the number of unrelated headers and SCP call sites that know
about the old PCRE 1 type.

## Other Prerequisites

### 1. Reliable PCRE2 build environment

The PCRE2 port will be difficult to validate if the test environment does
not consistently have the development headers and library available.

So we need:

- `libpcre2` development packages available in the relevant developer and
  CI environments
- one straightforward documented configuration that exercises the PCRE2
  path

### 2. Decide the transition point for deleting PCRE1

The project should not linger in a half-converted state longer than
necessary.

The intended sequence should be:

1. add missing tests and helper seams
2. port the runtime code to PCRE2
3. confirm the tests and builds pass
4. delete the PCRE1 path promptly

This is better than carrying a prolonged dual-backend support burden.

## Suggested Order

1. Add the missing unit tests:
   - case-insensitive regex
   - invalid regex syntax
   - no-regex-support behavior
   - capture-group cleanup
   - regex backend reporting and `SIM_REGEX_TYPE`
2. Refactor `scp.c` to expose small regex-reporting helpers
3. Refactor `scp_expect.c` to isolate compile/match/free operations
4. Port `sim_defs.h` and `scp_expect.c` to PCRE2
5. Update SCP help and build-report wording
6. Validate:
   - `WITH_REGEX=Off`
   - `WITH_REGEX=On -DWITH_PCRE2=On`
7. Remove the PCRE1 path and delete `WITH_PCRE2`

## End State

The desired end state is:

- PCRE2 is the only regex backend
- EXPECT regex support works exactly as it does today
- case-insensitive matching still works
- capture groups still populate `_EXPECT_MATCH_GROUP_n`
- backend reporting is accurate and tested
- the build no longer carries a deprecated-PCRE fallback path
