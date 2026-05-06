# SIM Timer API Cleanup

While expanding `sim_timer.c` unit coverage, we found that
`sim_timer_cancel()` was declared as returning `t_bool` but actually
returned status-style values such as `SCPE_OK` and `SCPE_IERR`. The
return type has been changed to `t_stat`, matching the values the
function already returned.

The remaining API cleanup is in `sim_cancel()`, which currently calls
`sim_timer_cancel()` but ignores the returned status. That caller should
be audited and changed deliberately if timer-cancel failures need to be
reported to callers.
