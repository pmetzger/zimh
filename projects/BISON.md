# Bison Regeneration Plan

The preserved parser grammar sources in this tree have already been
restored to active `.y` inputs.

Current files:

- `simulators/SAGE/m68k_parse.y`
- `simulators/AltairZ80/m68k/m68kasm.y`

Planned direction:

1. Add explicit Bison generation rules for both build systems.
   - CMake should generate parser outputs as part of the build.
   - The legacy Makefile should have explicit `bison` rules as well.
2. Remove the checked-in generated parser `.c` files once the build
   rules are in place and validated.
3. Add `bison` to the documented tool requirements in `BUILDING.md`.
4. Validate both build systems after the conversion.
   - Regenerate both parsers.
   - Rebuild the affected targets.
   - Confirm that changed generated output is acceptable.

Notes:

- Exact byte-for-byte reproduction of the current checked-in parser
  output is not required.
- Different Bison versions are expected to emit different boilerplate.
- The goal is maintainable regeneration from authoritative grammar
  sources, not preservation of historical generated text.
