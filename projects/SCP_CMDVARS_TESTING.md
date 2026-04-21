## SCP command-variable testing endgame

Current LLVM coverage for `src/core/scp_cmdvars.c` is:

- functions: `100%`
- lines: `100%`
- branches: `88.89%`

No new tools are needed for the remaining work. The existing environment
already has:

- `llvm-cov`
- `llvm-profdata`
- the `sim_time` test seam
- the `sim_dynstr` allocation-failure hook

### Completed since this note was created

The line-level endgame is complete, and a substantial part of the
remaining branch work is now done:

- `%*` allocation-failure paths are covered
- malformed `:~` substring parsing is covered
- the stale `/P` null-default branch is gone
- the stale trailing-space loop after `get_glyph_quoted()` is gone
- the `SIM_OSTYPE` seam now injects only the underlying `uname()` call,
  so the real helper is covered on both success and failure paths
- fixed-clock tests now cover:
  - ISO week/year boundaries
  - `DATE_19XX_*` leap-year cases
  - `DATE_D` Sunday handling
  - clock-failure fallback behavior
- `%~` filepath-part parsing now fails closed for malformed overlong part
  specifications instead of falling through into unrelated variable
  parsing
- many `_sim_get_env_special()` branches are now directly covered,
  including:
  - `TSTATUS`
  - `SIM_VERIFY` / `SIM_VERBOSE`
  - `SIM_QUIET` / `SIM_MESSAGE`
  - `SIM_BIN_NAME` / `SIM_BIN_PATH`
  - runlimit edge cases
  - fixup-buffer exhaustion

### Remaining work

What remains is branch coverage only. The current `llvm-cov` misses are
now mostly folded branch accounting from code structure, especially in:

- the long `_sim_get_env_special()` dispatcher chain
- short-circuit conditions like `tmnow && !strcmp(...)`
- compile-time platform splits
- helper internals where one branch side is folded away by the compiler

### Proposed order

1. refactor `_sim_get_env_special()` into smaller helpers, especially:
   - a time-variable helper that assumes `tmnow != NULL`
   - smaller non-time lookup helpers for the `SIM_*`, runlimit, and
     file-compare cases
2. rerun coverage after that refactor before adding more tests
3. only add tests for any branch sides that remain behaviorally distinct
   after the structural cleanup
4. decide whether the remaining folded branch misses are acceptable or
   whether further mechanical refactoring is worthwhile

### Goal

Finish `scp_cmdvars` to the project standard:

- complete automated confidence for the real behavior
- no dead branches kept only for coverage accounting
- narrow seams around real dependencies
- a deliberate distinction between meaningful uncovered behavior and
  branch-accounting residue
