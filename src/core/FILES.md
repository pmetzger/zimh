# `src/core` File Guide

This directory holds the core shared control-program and type-definition
layer for SIMH.

This file describes what each file is for and what kind of logic belongs
there. It intentionally does not inventory individual functions.

## `main.c`

This is the real process entrypoint. It should stay tiny and should only
bridge the host `main()` to the SCP entrypoint.

## `scp.c`

This is the main simulator control program implementation. It owns the
top-level command processor, interactive control flow, and the larger SCP
subsystems that have not been split into narrower helper modules.

## `scp.h`

This is the umbrella public header for the simulator control program. It
collects the shared SCP-facing interface and includes the narrower SCP
helper headers.

## `scp_breakpoint.c` and `scp_breakpoint.h`

These files own SCP's breakpoint subsystem: breakpoint-table management,
pending breakpoint actions, and the shared breakpoint status consumed by
simulators and the command layer.

## `scp_context.c` and `scp_context.h`

These files own simulator-context and lookup support extracted from
`scp.c`: device and unit naming, name lookup, internal-device
registration, and default-context bookkeeping.

## `scp_expect.c` and `scp_expect.h`

These files own the SCP expect/send subsystem: SEND and EXPECT command
handling, queued injected input, expect-rule matching, and the internal
device used to stop simulation when an expect rule fires.

## `scp_expr.c` and `scp_expr.h`

These files own SCP's shared expression parser and evaluator, including
the infix-to-postfix conversion and the expression-specific temporary
state needed while commands are being interpreted.

## `scp_help.c` and `scp_help.h`

These files own SCP's shared help runtime: the generic `HELP` output,
device-help rendering, and the structured hierarchical help parser used
by SCP and by devices that participate in that help system.

## `scp_parse.c` and `scp_parse.h`

These files own reusable SCP tokenization and switch-parsing support:
generic command-token handling, quoted-token handling, and simulator
switch parsing.

## `scp_size.h`

This header owns the small inline width-to-storage sizing policy shared by
the SCP modules.

## `scp_unit.c` and `scp_unit.h`

These files own generic unit-management helpers extracted from `scp.c`:
file-backed attach/detach behavior and ownership lookup starting from a
`UNIT *`.

## `sim_defs.h`

This is the primary shared-definition header for SIMH. It holds the core
types, structures, flags, and macros used across the codebase.

## `sim_printf_fmts.h`

This header centralizes `printf`-style format strings for SIMH's shared
types.

## `sim_rev.h`

This header owns revision and version metadata for the build.

## Placement Guidelines

When deciding where new core code should go:

- keep process-entrypoint glue in `main.c`
- keep top-level SCP control flow in `scp.c`
- prefer the narrower `scp_*` files for reusable extracted helpers
- use `sim_defs.h` only for truly shared core definitions
- add a new focused file instead of growing `scp.c` when a helper slice
  has a clear ownership boundary

If a change feels awkward in every existing file, that is usually a sign
that the code should get a new focused module instead of being forced
into `scp.c`.
