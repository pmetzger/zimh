## Remove SCP Internal State from the Process Environment

The current SCP design uses the host process environment as a general
internal variable store. That should be removed.

This is not a narrow `scp_expect.c` cleanup. It is a broader design
problem in `src/core`:

- internal runtime state is published with `setenv()`
- internal runtime state is cleared with `unsetenv()`
- SCP command substitution reads it back with `getenv()`
- tests seed and inspect process-environment state to validate command
  behavior

That is the wrong abstraction boundary.

The process environment should be used only for:

- genuine host-environment lookups
- explicit user-facing environment manipulation features
- child-process interaction where environment passing is actually the
  point

It should not be used as SCP's own mutable interpreter state.

### Why this is a problem

Using the process environment as an internal variable table is:

- process-global instead of subsystem-owned
- stringly typed
- easy to leak across tests
- easy to leave stale values behind
- hostile to parallelism and future concurrency
- difficult to reason about during debugging
- surprising for maintainers
- awkward for line-qualified state such as `TTY:1`
- dependent on host-environment semantics instead of interpreter
  semantics

The current situation is especially bad because the design couples
storage and expansion together:

1. SCP publishes internal state into the host environment
2. SCP command substitution reads from the host environment
3. internal features therefore depend on environment mutation to work

That whole loop should be broken.

## Scope of removal

This project should remove **all** SCP-owned internal state from the
process environment in `src/core`.

That includes the existing `scp_expect.c` uses, but it also includes the
broader cluster of `scp.c` variables that are currently published only so
SCP substitution can see them.

### Current internal uses to remove

#### 1. SEND/EXPECT defaults

`scp_expect.c` currently stores defaults in environment variables:

- `SIM_SEND_DELAY_*`
- `SIM_SEND_AFTER_*`
- `SIM_EXPECT_HALTAFTER_*`

including line-qualified forms such as:

- `SIM_SEND_DELAY_TTY_1`
- `SIM_SEND_AFTER_TTY_1`
- `SIM_EXPECT_HALTAFTER_TTY_1`

These are internal runtime defaults. They do not belong in the host
environment.

#### 2. EXPECT match results

When an EXPECT rule fires, `scp_expect.c` currently publishes:

- `_EXPECT_MATCH_PATTERN`
- `_EXPECT_MATCH_GROUP_0`
- `_EXPECT_MATCH_GROUP_1`
- ...

These are interpreter scratch variables used by later SCP command
actions. They should not be written into the process environment.

#### 3. RUNLIMIT state

`scp.c` currently publishes:

- `SIM_RUNLIMIT`
- `SIM_RUNLIMIT_UNITS`

This is command-interpreter runtime state and should become owned SCP
state instead.

#### 4. FILE COMPARE scratch state

`scp.c` currently uses:

- `_FILE_COMPARE_DIFF_OFFSET`

to publish internal FILE COMPARE results. This is another example of
interpreter scratch state being forced through the host environment.

#### 5. Regex/reporting metadata

`scp.c` currently publishes:

- `SIM_REGEX_TYPE`

This does not appear to justify any user-visible substitution surface at
all. Unless a real use case emerges, it should be removed rather than
replaced with an SCP-managed variable.

#### 6. Simulator identity and version metadata

`scp.c` currently publishes a substantial set of interpreter-visible
metadata with `setenv()`, including:

- `SIM_NAME`
- `SIM_BIN_NAME`
- `SIM_BIN_PATH`
- `SIM_MAJOR`
- `SIM_MINOR`
- `SIM_PATCH`
- `SIM_VM_RELEASE`
- `SIM_DELTA`
- `SIM_VERSION_MODE`
- `SIM_OSTYPE`
- `SIM_GIT_COMMIT_ID`
- `SIM_GIT_COMMIT_TIME`
- `SIM_ARCHIVE_GIT_COMMIT_ID`
- `SIM_ARCHIVE_GIT_COMMIT_TIME`

These are not host-environment configuration. They are internal SCP
variables being exposed through the wrong mechanism.

### Uses that are not in scope

The following are not the problem this project is trying to solve:

- reading real host environment such as `HOME`, `SHELL`, `ComSpec`,
  `OSTYPE`, processor-identification variables, and similar host facts
- explicit user-facing environment manipulation features such as `SET
  ENVIRONMENT`
- child-process environment handling around `system()` where SCP is
  actually interacting with an external command environment

Those may deserve their own cleanup later, but they are not the core
design bug here.

## The dependency that caused this design

The immediate reason these variables were shoved into the environment is
that SCP command substitution currently reads from the process
environment directly.

Relevant code in `scp.c` includes:

- `_sim_gen_env_uplowcase()`
- `%VAR%` substitution logic
- the initial-token expansion path that checks whether the first token is
  an environment variable

That means the current design is effectively:

- use the process environment as SCP's variable table
- expose internal state by calling `setenv()`
- make SCP substitution read it back with `getenv()`

This should be replaced with a real interpreter-owned variable system.

## Desired end state

SCP should have its own internal substitution mechanism, separate from
the host process environment.

That mechanism should not become a new generic stringly typed home for
all SCP state. Most of the current environment-backed values should
become ordinary typed runtime state in the structures that naturally own
them.

The process environment should remain available only as a separate host
environment source.

In the end state:

- typed SCP runtime state lives in its natural owning structures
- SCP command substitution resolves SCP-exposed variables without
  consulting the process environment
- host environment lookup remains available as a distinct capability
- internal commands no longer call `setenv()` merely to communicate with
  other SCP code

## Architectural direction

This cleanup should not replace "environment abuse" with "one giant
stringly typed internal variable table."

There are two distinct kinds of state here:

### 1. Natural typed runtime state

Most of the current environment use is actually owned runtime state that
should live in the obvious typed structures.

Examples:

- SEND default delay/after values
- EXPECT default haltafter values
- RUNLIMIT enabled/disabled state
- RUNLIMIT numeric limit and unit mode
- FILE COMPARE diff offset
- version/build metadata already available in program state

That state should be stored as typed data in the runtime objects or SCP
state that naturally own it.

### 2. Interpreter-visible substitution variables

Some values still need a string form because SCP command substitution
operates on names and expanded text.

Examples:

- `_EXPECT_MATCH_PATTERN`
- `_EXPECT_MATCH_GROUP_n`
- any `SIM_*` variables we still want visible to SCP substitution after
  the redesign

Those should live in a much smaller SCP-managed variable layer used only
for interpreter exposure and substitution.

This layer should be limited to values whose natural representation is
already textual or whose main purpose is ad hoc substitution.

### Resulting model

The intended model is therefore:

- typed state first
- provider-based substitution second
- a small string-valued SCP variable map only where strings are the
  natural representation
- host environment as a separate external source

The substitution layer should expose typed state when needed, but it
should not become the primary home for that state.

In particular, the preferred design is:

- typed providers for things like SEND defaults, RUNLIMIT state, version
  fields, and simulator identity
- a small internal string map for ad hoc interpreter variables such as
  EXPECT match captures
- host environment lookup only as a fallback external source

This avoids replacing one global string-based design with another.

It should also be selective about what remains substitutable at all.
Accidental substitutability caused by the current `getenv()` design
should not be preserved by default.

Keep substitutable:

- informational simulator/version/build metadata that is genuinely
  useful in command expansion
- explicit EXPECT match-result variables intended for command actions

Do not keep substitutable:

- SEND defaults
- EXPECT defaults
- FILE COMPARE scratch state
- internal control state such as RUNLIMIT, unless a concrete user-facing
  need is identified
- `SIM_REGEX_TYPE`, which should be removed unless a real consumer
  appears

## Proposed implementation plan

### Phase 1. Introduce an internal SCP substitution layer

Add an internal SCP substitution mechanism that can resolve names from
sources owned by SCP rather than from the process environment.

The preferred structure is:

- provider callbacks or equivalent logic for typed runtime state
- a small SCP-managed string variable map for ad hoc textual variables

If a string map is introduced, it should support at least:

- set by name
- unset by name
- lookup by name
- optional clear-by-prefix or grouped clear support

That string map is for ad hoc substitution values, not for general
runtime state storage. It should be owned by the SCP runtime and should
not depend on host-environment APIs.

### Phase 2. Teach SCP substitution to consult the internal SCP layer

Update SCP substitution so it can resolve names from the internal SCP
substitution layer.

This must cover:

- `%VAR%` substitution
- the "initial token expands if it names a variable" path

Design choice to settle explicitly:

- whether SCP internal variables shadow host environment variables
- or whether they live in separate namespaces

Recommended policy:

- resolve SCP-managed internal variables first for the names SCP owns
- allow host-environment fallback for genuine external variables

### Phase 3. Move SEND/EXPECT defaults into owned typed runtime state

Replace the SEND/EXPECT default environment-variable design with owned
state on the relevant runtime objects.

Likely ownership:

- console SEND defaults live on the console SEND context
- line SEND defaults live on the per-line SEND context
- console EXPECT defaults live on the console EXPECT context
- line EXPECT defaults live on the per-line EXPECT context

The existing `dev:line` split in `scp_expect.c` already provides the
natural ownership boundary.

### Phase 4. Move EXPECT match publication into the internal substitution
### layer

Publish:

- `_EXPECT_MATCH_PATTERN`
- `_EXPECT_MATCH_GROUP_n`

into the internal SCP substitution layer instead of the host
environment.

This phase must preserve current action semantics.

### Phase 5. Move other SCP-owned variables off `setenv()`

Migrate the other SCP-owned internal variables away from `setenv()`.

Where they represent real runtime state, move them into typed ownership.
Where they need to remain visible to SCP substitution, expose them
through typed providers or, if they are genuinely ad hoc textual
variables, publish them into the small SCP string variable map.

This includes:

- RUNLIMIT variables
- FILE COMPARE result variables
- simulator identity/version/build metadata that SCP exposes to command
  expansion

`SIM_REGEX_TYPE` should be deleted rather than migrated unless a real
consumer appears.

The goal is not to keep a special case for "most internal variables."
The goal is to stop using the process environment as SCP's internal
store altogether.

### Phase 6. Preserve explicit environment-facing features separately

Where SCP intentionally manipulates the host environment because the
feature is explicitly about the environment, keep that behavior as a
separate path.

Examples:

- `SET ENVIRONMENT`
- child-process launch behavior that needs real environment changes

Those should not be used as back doors for internal interpreter state.

### Phase 7. Rewrite tests

Existing unit tests currently use `getenv()`, `setenv()`, and
`unsetenv()` to validate SCP-owned behavior.

Rewrite those tests to assert against typed SCP state, provider-backed
substitution results, or the small SCP string variable map as
appropriate.

This includes:

- `tests/unit/src/core/test_scp_expect.c`
- `tests/unit/src/core/test_scp_regex.c`
- any other tests validating SCP-managed variables through the host
  environment

### Phase 8. Update documentation and help text

Update user-visible text that currently talks about environment
variables for SCP-owned internal state.

At minimum, update the SCP/EXPECT help text in `scp.c` so it no longer
claims that match state is stored in environment variables.

Also review documentation around:

- version/reporting variables
- RUNLIMIT-visible variables
- any other SCP-published internal names

Remove any user-facing mention of `SIM_REGEX_TYPE` if one exists.

## Compatibility questions to decide explicitly

These should be decided rather than left implicit:

- Should `%VAR%` still consult the host environment at all?
- If yes, what is the precedence between SCP variables and host
  environment variables?
- Should SCP-owned variables remain named like `SIM_*` and
  `_EXPECT_MATCH_*`, or should that naming be revised during migration?
- Do we need a user-facing command to inspect SCP internal variables once
  they are no longer piggybacking on the host environment?

My bias:

- keep host-environment lookup available for true external variables
- give SCP-owned variables their own real home
- keep the visible names initially for compatibility
- only rename them later if there is a clear reason

## Definition of done

This project is complete when all of the following are true:

- `src/core` no longer uses `setenv()` or `unsetenv()` for SCP-owned
  internal interpreter state
- SCP-owned variables no longer depend on `getenv()` for internal
  communication between commands and substitution logic
- SEND/EXPECT defaults no longer live in fabricated environment-variable
  names
- EXPECT match variables no longer live in the host environment
- RUNLIMIT, FILE COMPARE, regex reporting, and version/identity metadata
  are no longer published through `setenv()` merely to reach SCP
  substitution
- accidental substitution of SEND/EXPECT defaults, FILE COMPARE scratch
  state, and similar internal plumbing has been removed rather than
  preserved
- SCP substitution can resolve SCP-owned variables from an internal SCP
  substitution layer without using the process environment as storage
- tests no longer seed or inspect the host environment to validate
  SCP-owned state
- docs and help text no longer describe SCP internal state as environment
  variables unless they truly are user-facing host-environment features
