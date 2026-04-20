# PCRE2 Conversion

This project tracked the work needed to move ZIMH's SCP regular
expression support from the deprecated PCRE 1 API to PCRE2.

That conversion is now complete.

## Current State

The SCP EXPECT regex implementation now uses PCRE2.

The current model is simple:

- `WITH_REGEX=On` means PCRE2-backed regular expression support
- `WITH_REGEX=Off` means regex support is not built
- there is no backend-selection option
- the runtime tree no longer contains a PCRE 1 path

The main affected code paths are now:

- `src/core/sim_defs.h`
  - includes `pcre2.h` when regex support is enabled
  - defines `USE_REGEX` from `HAVE_PCRE2_H`
  - stores compiled regex state in `EXPTAB` as `pcre2_code *`
- `src/core/scp_expect.c`
  - uses `pcre2_compile`
  - uses `pcre2_pattern_info`
  - uses `pcre2_match`
  - uses `pcre2_code_free`
- `src/core/scp.c`
  - reports PCRE2 support in help and version output
  - publishes `SIM_REGEX_TYPE=PCRE2`
  - reports the backend version through `pcre2_config()`

## Test Coverage Added During The Conversion

The conversion work added or expanded coverage for:

- case-insensitive regex matching
- invalid regex syntax rejection
- capture-group cleanup across successive matches
- regex backend reporting
- `SIM_REGEX_TYPE` publishing

The focused unit targets for this area are:

- `simh-unit-scp-expect`
- `simh-unit-scp-regex`

## Notes

One real pre-existing bug was found and fixed during this work:

- stale `_EXPECT_MATCH_GROUP_n` values were not being cleared correctly
  after a later regex match with fewer capture groups

That fix is now covered by the SCP EXPECT unit tests.

## Remaining Follow-Up

There is no further backend-migration work left in this project.

Future work in this area, if any, should be ordinary maintenance:

- keep the SCP regex unit tests passing
- keep `BUILDING.md` dependency guidance aligned with the PCRE2 build
- add broader regression coverage only if new SCP regex features are added
