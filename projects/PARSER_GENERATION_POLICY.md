# Parser generation policy cleanup

This note tracks a build-system cleanup item exposed by Windows work.

## Problem

The maintained CMake build currently generates parser sources at normal
build time for at least:

- `simulators/AltairZ80/m68k/m68kasm.y`
- `simulators/SAGE/m68k_parse.y`

That means `bison` is a real build dependency today, not merely a tool
needed when developers choose to regenerate checked-in sources.

This drifted away from the documentation and made the Windows setup
story worse because common Windows packages provide `win_bison.exe`
instead of `bison.exe`.

## Cleanup direction

Choose one explicit policy and document it consistently:

1. Keep build-time parser generation.
2. Check in generated parser sources and regenerate them only when the
   grammar changes.

If policy 1 is kept:

- document `bison` as a normal build dependency
- keep Windows-specific executable-name support
- consider adding a clearer configure-time error that names
  `win_bison.exe`

If policy 2 is chosen:

- commit the generated parser outputs
- make `bison` optional for normal builds
- add an explicit regeneration path or option for maintainers

## Success criteria

- documentation matches actual build behavior
- Windows users do not fail on an avoidable executable-name mismatch
- the parser-generation dependency policy is explicit rather than
  accidental
