## SCP command-variable testing endgame

Current LLVM coverage for `src/core/scp_cmdvars.c` is:

- functions: `100%`
- lines: `98.52%`
- branches: `73.71%`

No new tools are needed for the remaining work. The existing environment
already has:

- `llvm-cov`
- `llvm-profdata`
- the `sim_time` test seam
- the `sim_dynstr` allocation-failure hook

### Remaining uncovered line-level work

The remaining uncovered lines are concentrated in a few small areas:

1. `%*` allocation-failure returns in `sim_cmdvars_expand_star_args()`

- lines 76, 85, and 87
- these should be covered by forcing `sim_dynstr` allocation failure from
  `test_scp_cmdvars.c`

2. quoted-line trailing-space skip in
   `sim_cmdvars_decode_initial_quoted_line()`

- line 113
- this should be covered with a whole-line quoted command followed by
  trailing spaces before end-of-line

3. `uname()` failure in `sim_cmdvars_probe_uname()`

- line 298
- current tests cover `SIM_OSTYPE` failure behavior through the existing
  probe hook, but that seam bypasses the real `sim_cmdvars_probe_uname()`
  helper

4. malformed `:~` substring parsing in `sim_subststr_substr()`

- lines 364-367
- this should be covered with a bad substring modifier that drives
  `sscanf()` into `case 0`

### Recommended cleanup before final coverage push

1. Add direct tests for the cheap remaining real paths.

- malformed `:~` substring parsing
- quoted whole-line decode with trailing whitespace
- `%*` allocation failure through the `sim_dynstr` test hook

2. Decide whether to refine the `SIM_OSTYPE` seam.

If we want honest line coverage of `sim_cmdvars_probe_uname()` itself, the
current seam is slightly too coarse because it replaces the whole probe
helper.

The likely improvement is:

- keep `sim_cmdvars_probe_uname()` real
- inject only the underlying `uname()` call through a tiny wrapper or
  function pointer

That would let tests cover both:

- success in the real helper
- failure in the real helper

without bypassing the code under test.

### Proposed order

1. add the cheap direct tests
2. re-run LLVM coverage
3. if `uname()` failure is still the only meaningful remaining gap, refine
   that seam and test it

### Goal

Finish `scp_cmdvars` to the project standard:

- complete automated confidence for the real behavior
- no dead branches kept only for coverage accounting
- remaining seams shaped narrowly around real dependencies rather than broad
  test bypasses
