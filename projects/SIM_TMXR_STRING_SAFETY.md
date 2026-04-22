# SIM TMXR String Safety

Goal: clean up the remaining string-safety issues in
`src/runtime/sim_tmxr.c` with a test-first approach.

This is the next `src/` slice in the string-safety audit because
`sim_tmxr.c` still contains a dense cluster of:

- `sprintf`
- `strcpy`
- `strncpy`
- ad hoc dynamic string growth

and much of that logic is central runtime code rather than simulator-local
code.

## Rule For This Slice

Before changing any string assembly or copy logic in `sim_tmxr.c`, add
enough unit coverage that the behavior is pinned down.

The target for phase 1 is not just "some tests around the touched
functions."  It is to drive `sim_tmxr` to full unit-test coverage for the
module's unit-testable logic, adding seams where needed so the behavior can
be exercised deterministically.

## Phase 1: Full Unit Coverage For sim_tmxr

Expand `tests/unit/src/runtime/test_sim_tmxr.c` from its current small
fixture-based smoke coverage into a comprehensive module test.

Focus areas:

- attach-string assembly
  - `tmxr_mux_attach_string`
  - `tmxr_line_attach_string`
  - ordinary cases
  - long-string growth cases
  - optional fields and separators

- formatted message output
  - `tmxr_linemsg`
  - `tmxr_linemsgf`
  - `tmxr_linemsgvf`
  - CR/LF handling
  - buffered-send retry behavior
  - long formatted messages

- connection/disconnection reporting
  - `tmxr_report_connection`
  - `tmxr_report_disconnection`

- line selection and descriptor lookup helpers
  - `tmxr_find_ldsc`
  - `tmxr_get_ldsc`
  - valid and invalid line references

- line-order parsing and display
  - `tmxr_set_lnorder`
  - `tmxr_show_lnorder`
  - malformed input cases

- summary/status formatting
  - `tmxr_fconns`
  - `tmxr_fstats`
  - `tmxr_show_summ`
  - `tmxr_show_cstat`

- debug-buffer escaping
  - `_tmxr_debug`
  - telnet option escaping
  - octal escaping
  - printable vs control-character cases

- queue-length helpers and line state helpers
  - keep and extend the existing coverage

Where real sockets or host I/O would otherwise be required, add narrow test
seams around the host interaction instead of leaving coverage holes.

## Phase 2: Structural Cleanup That Helps Testing

Once the module is well covered, simplify the existing string-building
machinery before changing individual call sites.

Main targets:

- replace the local `growstring` pattern with `sim_dynstr` where that makes
  the code clearer
- isolate repeated formatted-append logic into smaller helpers
- separate host/socket effects from pure string assembly
- widen `TMXR_LINE_DISABLED` (or a related public helper/constant) into
  `sim_tmxr.h` so tests and callers do not have to know that the private
  sentinel value is `-1`
- separate pure parse/validate logic in `tmxr_set_lnorder()` from user-message
  emission where possible, so tests can assert on bare status values without
  depending on `sim_messagef` decoration
- review and likely clean up odd attach-string edge behavior once coverage is
  in place, especially the current leading-comma `,Connect=...` result for
  some single-line cases
- consider factoring `tmxr_fstats()` / `tmxr_fconns()` string formatting from
  the thin `tmxr_show_cstat()` wrapper, because status-output tests quickly
  become coupled to incidental formatting details otherwise
- consider whether `tmxr_fstats()` should continue reporting the
  speed-throttled `tmxr_rqln()` view instead of raw buffered-character count;
  today the status display is coupled to timing policy, which is surprising in
  tests and may be surprising to users too
- split `tmxr_set_log()` / `tmxr_set_nolog()` responsibilities with at least a
  small helper for refreshing the owning mux attach string after state changes
- keep the current `mp->uptr->filename` attach-string regeneration contract in
  mind as a design smell to revisit later; it works today, but it overloads a
  filename slot with synthetic configuration text
- keep the now-fixed port-speed-control attach contract explicit in the code:
  the mode is configurable only while the mux is unattached, and later cleanup
  should preserve that behavior unless we deliberately redesign it
- factor the repeated mux-wide state-setter pattern used by
  `tmxr_set_notelnet_state()` and `tmxr_set_nomessage_state()`, which both
  duplicate the same “reject if any line is attached” scan and
  propagate-to-all-lines loop
- consider breaking `tmxr_attach_help()` into a few focused helpers
  (common intro, single-line specifics, multi-line specifics, speed/config
  section) once coverage is good enough to support that refactor

The goal is to make the later `sprintf`/`strcpy` migration mechanical and
safe, not to do a large redesign.

## Phase 3: String-Safety Migration

After coverage and structural cleanup:

- replace `sprintf` with bounded formatting or `sim_dynstr` assembly
- replace fixed-buffer `strcpy`/`strncpy` use with explicit bounded copies
  where fixed buffers remain appropriate
- preserve exact externally visible formatting unless there is a deliberate
  cleanup with tests updated to match

## Immediate Next Steps

1. Measure current `sim_tmxr.c` unit coverage in the LLVM build.
2. Expand `test_sim_tmxr.c` to cover the attach-string and formatted-message
   helpers first.
3. Add narrow seams for any remaining host/socket behavior blocking full
   unit coverage.
4. Only then begin the actual string-safety code changes in `sim_tmxr.c`.

## Ongoing Rule

If new architectural cleanup opportunities are discovered while expanding
coverage or doing the string-safety migration, add them here instead of
silently deferring them.
