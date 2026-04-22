# SIM TMXR String Safety

Goal: finish the remaining string-safety cleanup in
`src/runtime/sim_tmxr.c` with the current unit and integration coverage in
place.

What is already done:

- substantial TMXR unit coverage expansion
- `TMXR_LINE_DISABLED` widened into `sim_tmxr.h`
- the inverted port-speed-control attach check fixed
- attach-string assembly moved off the old local `growstring` helper
- generic dynamic-string work moved into `sim_dynstr`

What remains:

- continue replacing remaining `sprintf` use with bounded formatting or
  `sim_dynstr` assembly
- replace fixed-buffer `strcpy` / `strncpy` use with explicit bounded copies
  where fixed buffers still make sense
- isolate repeated formatted-append logic where it still exists
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

Rule:

- If new architectural cleanup opportunities are discovered while expanding
  coverage or doing the string-safety migration, add them here instead of
  silently deferring them.
