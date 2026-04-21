# SCP Extraction Candidates

This note records the next good candidates for extracting focused
components from `src/core/scp.c`.

Current context:

- `scp.c` is still about 12,500 lines long.
- `src/core/FILES.md` already establishes the intended pattern:
  narrow reusable SCP subsystems should move into focused `scp_*`
  modules instead of continuing to accumulate in `scp.c`.
- Recent work already made some seams cleaner, especially around
  substitution and environment handling.

## Recommended Order

The best next extraction order is:

1. substitution and environment layer
2. command-file control flow
3. host file-tool commands

After that, the next candidates are:

4. attach, detach, assign, save, and restore command layer
5. SHOW and SET command plumbing

## 1. Substitution and Environment Layer

This is probably the cleanest seam now.

Relevant code in `src/core/scp.c`:

- internal substitution-variable map around lines 623 to 699
- `_sim_get_env_special()` around line 3963
- `sim_sub_args()` around line 4212
- `sim_unsub_args()` around line 4366
- `sim_set_environment()` around line 5074

Why this is a good candidate:

- it is already conceptually one subsystem
- recent environment-cleanup work clarified its boundaries
- it has a small internal state model of its own
- it is used by the command interpreter broadly, but it is not tightly
  coupled to simulator execution

Suggested destination:

- `scp_subst.c`
- `scp_subst.h`

## 2. Command-File Control Flow

This is the largest coherent interpreter subsystem still living in
`scp.c`.

Relevant commands:

- `do_cmd()` around line 3399
- `assert_cmd()` around line 4540
- `sleep_cmd()` around line 4742
- `goto_cmd()` around line 4789
- `return_cmd()` around line 4847
- `shift_cmd()` around line 4859
- `call_cmd()` around line 4870
- `on_cmd()` around line 4888

Why this is a good candidate:

- it is really SCP script execution and control flow
- it is much more cohesive internally than its current placement
  suggests
- it is a large block with a fairly obvious ownership boundary

Suggested destination:

- `scp_flow.c`
- or `scp_script.c`

## 3. Host File-Tool Commands

This is a nice command-family cluster with a clear user-facing theme.

Relevant commands:

- `pwd_cmd()` around line 6569
- `dir_cmd()` around line 6640
- `type_cmd()` around line 6712
- `delete_cmd()` around line 6789
- `copy_cmd()` around line 6841
- `rename_cmd()` around line 6861
- `mkdir_cmd()` around line 6877
- `rmdir_cmd()` around line 6916

Why this is a good candidate:

- these are effectively SCP host file-utility commands
- they are related to one another more than to the rest of `scp.c`
- they should be straightforward to move without disturbing the core
  interpreter

Suggested destination:

- `scp_filecmd.c`
- `scp_filecmd.h`

## 4. Attach, Detach, Assign, Save, Restore Command Layer

This is another coherent command-family cluster.

Relevant commands:

- `attach_cmd()` around line 7394
- `detach_cmd()` around line 7448
- `assign_cmd()` around line 7525
- `deassign_cmd()` around line 7563
- `save_cmd()` around line 7592
- `restore_cmd()` around line 7763

Why this is a good candidate:

- `scp_unit.c` already owns lower-level generic unit helpers
- this cluster is the command-facing storage and media layer above those
  helpers
- the ownership split would become clearer if the command layer moved
  too

This is a good candidate, but not as clean a first move as substitution
or script control flow.

## 5. SHOW and SET Command Plumbing

Relevant commands:

- `set_cmd()` around line 5164
- `show_cmd()` around line 5450

Why this is worth considering:

- these are still large and central
- they are command-layer plumbing rather than top-level SCP control flow

Why it is lower priority:

- this area is more entangled with command tables, device metadata, and
  modifier rendering
- it is likely to require more careful staging than the higher-priority
  candidates above

## Lower-Priority Candidates

These are real seams, but they are not the best next moves.

### Run and Debug Layer

Relevant command:

- `run_cmd()` around line 8188

Reason to defer:

- this is more entangled with simulator execution, stop reasons, and
  breakpoint behavior
- `scp_breakpoint.c` already extracted part of the surrounding area, so
  the remaining seam is less clean

### Small Utility Command Cluster

Relevant commands:

- `help_cmd()` around line 3139
- `spawn_cmd()` around line 3294
- `screenshot_cmd()` around line 3320
- `echo_cmd()` around line 3329

Reason to defer:

- these are relatively small
- extracting them now would not reduce `scp.c` nearly as much as the
  higher-value candidates

### Command Lookup Tables

Examples:

- `find_cmd`
- `find_ctab`
- `find_c1tab`
- `find_shtab`

Reason to defer:

- important, but not yet as clean a seam as substitution or script flow
- likely to be more invasive than the first few extractions need to be

## Recommendation

The best next steps are:

1. extract the substitution and environment layer into `scp_subst.c`
2. extract command-file control flow into `scp_flow.c`
3. extract host file-tool commands into `scp_filecmd.c`

That order gives the best balance of:

- clear ownership
- lower extraction risk
- meaningful reduction in `scp.c`
- continued movement toward the module layout already described in
  `src/core/FILES.md`
