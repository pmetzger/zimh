## SCP EXPECT Refactoring Opportunities

The recent unit-coverage work around `scp_expect.c` exposed several
places where small refactorings would make the code easier to test and
reason about.

### 1. Split command target resolution from command parsing

`send_cmd()`, `sim_show_send()`, `expect_cmd()`, and `sim_show_expect()`
each do three things at once:

- parse an optional `dev:line` prefix
- resolve that prefix to a `SEND *` or `EXPECT *`
- parse and execute the command

Those responsibilities should be split. A small helper that resolves a
console-or-line target would make the TMXR paths testable without having
to drive the full command parser each time.

### 2. Share SEND/EXPECT environment-variable name construction

`get_default_env_parameter()` and `set_default_env_parameter()` both
build the same `SIM_SEND_*` and `SIM_EXPECT_*` variable names, including
the `dev:line` form.

That duplication makes it harder to test the line-qualified path and
invites drift. A small helper that formats the variable name once would
reduce that risk and make direct unit tests possible.

### 3. Split exact-match buffer handling in `sim_exp_check()`

The non-regex matching path in `sim_exp_check()` still contains several
different sub-cases inline:

- not enough buffered data yet
- wrap-around matching
- contiguous-buffer matching
- match-triggered cleanup and activation

Those branches are now covered better than before, but they are still
hard to follow because the control flow is packed into one function.
Breaking the exact-match path into focused helpers would make both the
implementation and the tests easier to maintain.

### 4. Separate SHOW formatting from state gathering

`sim_show_send_input()` and `sim_exp_show()` mix:

- collection of dynamic state
- default-value lookup
- text formatting

This makes it easy for reporting bugs to hide behind accidental equality
of values, as happened with the SEND default-delay reporting bug. Small
formatting helpers that accept explicit values would make those paths
much easier to test precisely.

### 5. Introduce TMXR fixtures for line-qualified SEND/EXPECT tests

The remaining meaningful untested functional paths are mostly the
`dev:line` branches. Reaching them cleanly will likely require a small
test fixture that opens a minimal TMXR multiplexer and exposes one or
two active lines.

That fixture would unlock direct coverage for:

- `tmxr_locate_line_send()` paths
- `tmxr_locate_line_expect()` paths
- environment-variable names in `DEV:LINE` form
- console-vs-line reporting differences

### 6. Keep debug-only branches isolated

Several remaining branches are only reachable with `sim_deb` and
debug-bit setup. Those paths are legitimate, but they add a lot of
branch noise to coverage reports.

If debug-only formatting logic were isolated more cleanly from the
normal control flow, the core behavior would be easier to cover
completely without having to build elaborate debug fixtures.
