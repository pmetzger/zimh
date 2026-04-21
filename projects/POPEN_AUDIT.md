# Popen Audit

## Goal

Find uses of `popen()` that are really asking the host operating system
for information and replace them with direct OS APIs where practical.

The main motivation is:

- simpler code
- fewer shell dependencies
- no `PATH` dependence for basic host introspection
- fewer quoting and parsing failure modes
- better portability discipline inside SCP and runtime support code

This is not a blanket "remove all `popen()` calls" project. Some uses
may still be legitimate if they are intentionally running an external
tool or shell pipeline as part of the feature.

## Current Uses

The current tree contains these `popen()` call sites:

- `src/core/scp.c`
  - `find` / version shell commands around lines 5028, 5039, 5063
  - host information probes around lines 5281, 5291, 5301, 5321
- `src/runtime/sim_ether.c`
  - external command execution around lines 1673, 1682
  - hostname probe around line 1772

## High-Value Replacements

These look like the best first candidates for replacement with direct
system interfaces:

- `src/core/scp.c`
  - `uname -a`
  - `uname`
  - `lscpu ... | grep 'Model name:'`
  - `sysctl -n machdep.cpu.brand_string`

- `src/runtime/sim_ether.c`
  - `hostname`

Likely replacement directions:

- `uname(3)` for OS and kernel identity
- `sysctl(3)` on macOS and BSD where appropriate
- Linux interfaces such as `/proc/cpuinfo` only if no better portable
  API exists for the specific information needed
- `gethostname(3)` for host name

## Lower-Priority Or Possibly Legitimate Uses

These may be legitimate shell-command features rather than accidental
host introspection:

- `src/core/scp.c`
  - the `findcmd` uses
  - the `versioncmd` use
- `src/runtime/sim_ether.c`
  - the `command`-driven external invocations

These should be reviewed before changing anything. If they are part of a
feature that intentionally runs a host command, they should probably
stay as they are.

## Plan

1. Audit each `popen()` use and classify it as one of:
   - host introspection
   - intentional external command execution
   - unclear; needs design decision

2. Replace the clear host-introspection cases first:
   - SCP host-information probes in `scp.c`
   - hostname probe in `sim_ether.c`

3. Add or extend unit tests where the code can be made testable with a
   small seam, as was done for `SIM_OSTYPE`.

4. Leave command-execution cases alone unless we decide the feature
   itself should change.

## Definition Of Done

- all host-introspection uses of `popen()` in maintained code are
  replaced with direct OS APIs or clearly justified platform helpers
- remaining `popen()` calls are only intentional external-command uses
- tests cover the new logic where practical
