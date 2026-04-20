## Add an Explicit SCP Temporary File Facility

SCP currently has no explicit user-facing facility for creating a
temporary file name from within the command interpreter.

That gap shows up when command files need scratch file paths for:

- transient logs
- intermediate output
- tool handoff through `SPAWN`
- temporary save/compare inputs

At the moment, scripts would have to:

- construct names themselves from unrelated substitution variables, or
- ask the host shell to do it

Neither is a good SCP-level design.

This should be solved with an explicit interpreter feature rather than
by exposing low-level host-environment details or reviving accidental
substitution variables such as `%UTIME%`.

## Goals

Add a supported way for SCP command files to obtain a unique temporary
file path.

The facility should:

- work on all supported host platforms
- use the platform's normal temporary-file location rules
- avoid name collisions
- avoid pushing new state through the host process environment
- be documented as a normal SCP feature

## Desired user model

The feature should be explicit and unsurprising.

Candidate designs:

- a built-in substitution variable with on-demand generation
- a dedicated command that assigns a generated path to an SCP variable
- a `SET`/`SHOW`-style subcommand in the SCP variable system once that
  exists

The safest likely direction is a command that writes into an explicit SCP
variable name, because it avoids hidden regeneration behavior.

For example, something conceptually like:

- generate a temp path
- store it in an SCP-managed variable
- let later commands reference that variable normally

Exact syntax can be decided later.

## Things this feature should not do

This facility should not:

- depend on the host environment as SCP's internal variable store
- require shelling out just to obtain a path
- expose low-value time fragments or version metadata merely so scripts
  can fake unique names
- promise persistence beyond the life of the current SCP session

It also should not silently create the file unless that becomes an
explicit part of the design. Generating a path and creating a file are
different operations.

## Implementation considerations

The implementation should use normal platform facilities for temporary
paths and secure unique-name creation.

Questions to settle:

- should SCP generate only a path, or create the file too?
- should the resulting name be automatically cleaned up anywhere?
- should the feature support an optional prefix/suffix?
- should it be tied to the future SCP-owned variable mechanism from
  `projects/SCP_ENVIRONMENT_REMOVAL.md`?

The last point likely matters. If SCP is moving toward its own variable
mechanism, this feature should target that mechanism instead of adding
new dependence on `%ENVVAR%` expansion.

## Suggested order

1. Decide the user-facing interface.
2. Implement secure cross-platform temporary-path generation.
3. Publish the result through the intended SCP variable mechanism.
4. Add unit tests for:
   - generated path is non-empty
   - repeated generation produces different names
   - names land in an appropriate temp area
5. Document the feature in `users_guide.md`.

## Definition of done

This project is complete when:

- SCP has a documented temporary-file-name facility
- scripts no longer need shell workarounds for this use case
- no new host-environment abuse is introduced
