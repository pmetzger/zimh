## Temporary File API Migration Backlog

ZIMH now has `sim_tempfile_open()` and `sim_tempfile_open_stream()` for
secure cross-platform temporary file creation. New simulator and runtime
code should prefer these helpers instead of calling `mkstemp`,
`mkstemps`, `mktemp`, or ad hoc temporary path construction directly.

The goal of this backlog is to finish moving in-tree C callers to the
shared API and then decide whether the Windows `mkstemp` compatibility
shim can be removed.

## Current State

Production code using the shared API:

- `src/core/scp_help_engine.c`
- `simulators/Ibm1130/ibm1130_cr.c`

Known production temporary-file users that still need review:

- `src/runtime/sim_frontpanel.c`
  - Builds a temporary simulator configuration path as
    `<sim_config>-Panel-<pid>`.
  - Creates it with plain `fopen("w")`.
  - Should be moved to `sim_tempfile_open_stream()` after focused tests
    cover the generated configuration path.
- `src/runtime/sim_console.c`
  - Creates a remote-console temporary log named
    `sim_remote_console_<pid>.temporary_log`.
  - The name is process-ID based and appears to live in the current
    working directory.
  - Should be reviewed for conversion to `sim_tempfile_open_stream()` or
    a small wrapper around it, with tests for cleanup on command
    completion and master-mode/session cleanup.

Related temporary-directory exposure:

- `src/core/scp_cmdvars.c`
  - Exposes `%SIM_TMPDIR%` through `sim_host_temp_dir()`.
  - This is not a temporary-file creator, but it should remain aligned
    with the directory selection used by `sim_tempfile_*`.

Compatibility shim still present:

- `src/compat/tempfile.c`
- `src/compat/sim_win32_compat.h`

Known direct `mkstemp` or `mkstemps` users are currently tests, not
simulator production code.

## Backlog

### 1. Move front-panel temporary configuration files

`src/runtime/sim_frontpanel.c` should stop constructing
`<sim_config>-Panel-<pid>` by hand. That path is predictable, lives next
to the input config instead of the host temp directory, and depends on a
plain `fopen()` create.

Before changing it, add focused tests around the front-panel generated
configuration path:

- temporary config file is created successfully
- generated config includes the original config contents and injected
  remote console commands
- the temporary path is cleaned up by existing panel teardown paths
- failures leave useful errors

Then switch creation to `sim_tempfile_open_stream()`.

### 2. Move remote-console temporary logs

`src/runtime/sim_console.c` creates a temporary log for remote-command
output when logging is not already active. The current name is based on
the process ID and does not use the host temporary directory API.

Before changing it, add tests for:

- temporary log creation when no log is active
- cleanup after command completion
- cleanup through master-mode/session cleanup paths
- behavior when temporary log creation fails

Then route creation through the shared temporary-file API. This may need
a small seam around `sim_set_logon()`, because that API currently opens
the log itself from a path rather than accepting an already-open stream.

### 3. Move named temporary-file test fixtures

Move C tests that use `mkstemp` or `mkstemps` merely to create named
temporary fixtures to `sim_tempfile_*`. This is useful cleanup because
it removes incidental dependence on the compatibility shim and makes the
tests follow the same temporary-file path as production code.

Keep the tests that intentionally cover the compatibility shim:

- `tests/unit/src/compat/test_compat_temp.c`

Focused migration candidates:

- `tests/unit/src/core/test_scp_cmdvars.c`
- `tests/unit/src/runtime/test_sim_tmxr.c`

After these are migrated, the only in-tree C `mkstemp` and `mkstemps`
callers should be the compatibility tests themselves. That gives us a
clear decision point for keeping or removing the shim.

### 4. Consider whether `tmpfile()` test fixtures should move

Many unit tests use `tmpfile()` for anonymous scratch streams. That is
usually fine when the test does not need a visible path, does not need
to test cleanup, and does not need Windows-specific file sharing
behavior.

Do not migrate anonymous stream fixtures mechanically. Migrate them only
when a test needs a real path, when Windows behavior matters, or when
using the shared API makes the tested behavior clearer.

### 5. Leave shell-script `mktemp` uses separate

The shell scripts that call host `mktemp` are not C runtime users:

- `tools/dev/extract-word-text.sh`
- `tests/manual/run-integration-tty-check.sh`

These should be reviewed separately as shell portability work. They are
not blockers for removing C callers from the Windows compatibility shim.

### 6. Remove or narrow the compatibility shim when unused

After in-tree C callers no longer depend on `mkstemp` or `mkstemps`
outside the compatibility tests, decide whether to:

- remove `src/compat/tempfile.c` and the declarations from
  `src/compat/sim_win32_compat.h`, or
- keep the shim only as a compatibility layer for external or legacy
  code that may still include SIMH/ZIMH headers.

If the shim stays, document why it remains. If it is removed, remove the
dedicated compatibility tests at the same time.

## Definition of Done

- Production C code creates temporary files through `sim_tempfile_*`.
- Non-compatibility tests use `sim_tempfile_*` when they need named
  temporary files.
- Compatibility tests are either retained deliberately or removed with
  the shim.
- `src/compat/README.md` accurately describes the final state.
