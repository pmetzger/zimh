# Core sprintf Cleanup

The MacPorts Clang warning build still reports deprecated `sprintf()`
use in `src/core`. These should be repaired deliberately rather than as
drive-by edits in unrelated changes.

Current instances are in:

- `src/core/scp.c`
- `src/core/scp_breakpoint.c`
- `src/core/scp_cmdvars.c`
- `src/core/scp_context.c`
- `src/core/scp_expect.c`

Each repair should preserve existing formatting behavior under focused
unit coverage before replacing `sprintf()` with bounded formatting or a
more appropriate string helper. Some of these sites append into existing
buffers or format user-visible command text, so the right fix is not
always a mechanical `snprintf()` substitution.
