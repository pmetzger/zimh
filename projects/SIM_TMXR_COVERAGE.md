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
  pure parse/normalize helpers, de-duplicated listener-option / ACL parsing,
  and use of the TMXR-local sleep hook for its temporary port-validation
  open/close cycle
- `tmxr_report_connection()` still does a direct `sim_os_ms_sleep()` in the
  serial-pending path before it marks the line connected, so that path wants
  the same testability treatment as the newer reset/reconnect helpers
- the packet APIs still have an internal loopback inconsistency:
  `tmxr_put_packet_ln(_ex)` accepts loopback lines, but the lower
  `tmxr_putc_ln()` path rejects notelnet loopback traffic unless `conn` is
  also true, so packet loopback tests currently have to model an established
  line explicitly
- `tmxr_poll_conn()` still has one direct `sim_connect_sock_ex()` call in its
  outgoing-initiation path instead of using the TMXR-local hook, which is a
  testability gap and an inconsistency in the new seam
- incoming listener acceptance through `tmxr_poll_conn()` now has the hooks it
  needs, but verifying retained peer-address state still seems to want a more
  realistic listener fixture than the current stripped-down unit setup
- `tmxr_locate_line*()` and the line-name helpers depend on the global
  registered-device table and real `SEND` / `EXPECT` ownership, so eventual
  unit coverage there likely wants either a narrow `find_dev()` seam or tests
  that run against a real registered TMXR device rather than the local fixture

Rule:

- If new architectural cleanup opportunities are discovered while expanding
  coverage or doing the remaining TMXR cleanup, add them here instead of
  silently deferring them.
