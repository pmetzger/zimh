## SCP command-variable testing endgame

Current LLVM coverage for `src/core/scp_cmdvars.c` is:

- functions: `100%`
- lines: `100%`
- branches: `75.28%`

No new tools are needed for the remaining work. The existing environment
already has:

- `llvm-cov`
- `llvm-profdata`
- the `sim_time` test seam
- the `sim_dynstr` allocation-failure hook

### Recommended cleanup before final coverage push

The line-level endgame is complete:

- `%*` allocation-failure paths are covered
- malformed `:~` substring parsing is covered
- the stale `/P` null-default branch is gone
- the stale trailing-space loop after `get_glyph_quoted()` is gone
- the `SIM_OSTYPE` seam now injects only the underlying `uname()` call,
  so the real helper is covered on both success and failure paths

### Remaining work

What remains is branch coverage, not missing lines. The next question is
which branch misses are real behavior gaps and which are just accounting
noise from:

- loop exits
- compound conditions with already-covered lines
- platform-specific compile-time paths
- low-value combinations in the built-in variable dispatcher

### Proposed order

1. audit remaining branch misses in `sim_sub_args()` and
   `_sim_get_env_special()`
2. identify any branch misses that represent genuinely distinct behavior
3. add tests only for those meaningful remaining cases
4. treat platform-exclusive or pure accounting misses separately so the
   final work stays intentional rather than mechanical

### Goal

Finish `scp_cmdvars` to the project standard:

- complete automated confidence for the real behavior
- no dead branches kept only for coverage accounting
- narrow seams around real dependencies
- a deliberate distinction between meaningful uncovered behavior and
  branch-accounting residue
