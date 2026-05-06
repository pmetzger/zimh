# Dynamic Strings

Fixed-size string buffers still exist in command parsing and substitution
paths.  They are large enough for many ordinary simulator commands, but
they create bad behavior when expansion results exceed the buffer:
truncation can happen silently, and users may get mysterious shortened
paths, environment values, or command strings instead of a clear error.

The `scp_cmdvars` substitution fixup bug is one example.  The immediate
repair makes truncation memory-safe and NUL-terminated, but the API still
cannot report that a value was shortened.

Long term, command-variable expansion and nearby SCP string handling
should move to dynamic strings.  Internal helpers should build complete
results, propagate allocation failures, and report truncation only at
well-defined command boundaries where truncation is truly part of the
interface.

The existing `sim_dynstr` module is sufficient for most of this work.
It already supports appending strings, characters, and formatted text,
transferring ownership, and allocator failure hooks for tests.  We may
want small convenience helpers, but this does not require inventing a
new string library.

The likely migration should happen in stages:

1. Add dynamic internal helpers while preserving the current public APIs.
   For example, `sim_sub_args()` can build the complete expansion in a
   `sim_dynstr_t`, then copy into the caller buffer at the compatibility
   boundary.  This removes most internal truncation but still preserves
   legacy final truncation.

2. Add status-returning APIs that can report truncation and allocation
   failure.  Keep the old `void sim_sub_args()` wrapper during
   migration, but move callers toward an API that does not hide errors.

3. Redesign `_sim_get_env_special()` so ownership is explicit.  Today it
   may return the caller buffer, a host environment pointer, an internal
   variable pointer, or `NULL`.  A dynamic result or append-into-dynstr
   interface would make lookup, fixup, truncation, and allocation
   failures much clearer.

The hard part is not basic string growth; it is preserving contracts
while changing them.  `sim_sub_args()` currently mutates in place and
returns `void`, and `sim_unsub_args()` depends on source-position mapping
through `sim_sub_instr_off`.  A dynamic rewrite must preserve that source
mapping behavior or replace it with a documented equivalent.

Priority areas:

- `sim_sub_args()` and `_sim_get_env_special()` in `src/core/scp_cmdvars.c`
- command parsing paths that pass `CBUFSIZE` or `4 * CBUFSIZE` buffers
- environment, path, and wildcard/argument expansion code that can
  produce results larger than the original command text

Completed:

- `src/runtime/sim_tape.c` ANSI and DOS11 directory-entry path
  construction now uses dynamic strings instead of `PATH_MAX` buffers.
- `src/runtime/sim_tape.c` self-test file names and attach arguments now
  use dynamic strings instead of fixed local buffers.
