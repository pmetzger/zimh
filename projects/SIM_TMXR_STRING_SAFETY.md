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
- `tmxr_report_connection()` now uses a pure connection-message helper
- the reset/reconnect paths now have a narrow TMXR-local host-I/O seam for
  deterministic unit tests
- the TMXR-local host-I/O seam now also covers key `tmxr_poll_conn()` socket
  operations (`accept`, connect-state checks, peer-name lookup, and writes)
- the remaining raw `sprintf` / `strcpy` cleanup in `sim_tmxr.c` has been
  burned down

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
- `tmxr_open_master()` now dominates the remaining string-safety backlog, and
  it likely wants pure parse/normalize helpers before more string-function
  replacement
- the packet APIs still have an internal loopback inconsistency:
  `tmxr_put_packet_ln(_ex)` accepts loopback lines, but the lower
  `tmxr_putc_ln()` path rejects notelnet loopback traffic unless `conn` is
  also true, so packet loopback tests currently have to model an established
  line explicitly
- incoming listener acceptance through `tmxr_poll_conn()` now has the hooks it
  needs, but verifying retained peer-address state still seems to want a more
  realistic listener fixture than the current stripped-down unit setup
- `tmxr_locate_line*()` and the line-name helpers depend on the global
  registered-device table and real `SEND` / `EXPECT` ownership, so eventual
  unit coverage there likely wants either a narrow `find_dev()` seam or tests
  that run against a real registered TMXR device rather than the local fixture

Rule:

- If new architectural cleanup opportunities are discovered while expanding
  coverage or doing the string-safety migration, add them here instead of
  silently deferring them.
