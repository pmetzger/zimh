# SIM TMXR Coverage

Goal: finish the remaining coverage and cleanup work in
`src/runtime/sim_tmxr.c` with deterministic unit tests and stable
integration coverage.

What remains:

- separate host/socket effects from pure string assembly where that clarifies
  the code
- separate pure parse/validate logic in `tmxr_set_lnorder()` from message
  emission where practical
- review the odd leading-comma `,Connect=...` attach-string behavior for some
  single-line cases
- consider factoring `tmxr_fstats()` / `tmxr_fconns()` formatting away from
  `tmxr_show_cstat()`
- consider whether `tmxr_fstats()` should report raw buffered count instead of
  the speed-throttled `tmxr_rqln()` view
- split `tmxr_set_log()` / `tmxr_set_nolog()` responsibilities with a helper
  for refreshing the owning mux attach string
- revisit the `mp->uptr->filename` synthetic attach-string contract
- factor the repeated mux-wide state-setter pattern used by
  `tmxr_set_notelnet_state()` and `tmxr_set_nomessage_state()`
- consider breaking `tmxr_attach_help()` into a few focused helpers
- `tmxr_open_master()` now dominates the remaining backlog; it likely wants
  more pure parse/normalize helpers
- mux-wide listener acceptance through `tmxr_poll_conn()` still seems to want
  a more realistic listener fixture before retained peer-address state can be
  unit-tested confidently; line-specific listener acceptance is now covered

Rule:

- If new architectural cleanup opportunities are discovered while expanding
  coverage or doing the remaining TMXR cleanup, add them here instead of
  silently deferring them.
