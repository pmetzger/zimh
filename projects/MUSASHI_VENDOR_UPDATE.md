# Replace Embedded Musashi With Upstream Vendor Import

## Goal

Replace the embedded Musashi Motorola 68K emulator sources in this tree with a
clearly vendored copy of the maintained upstream project:

https://github.com/kstenerud/Musashi

The end state should make it obvious which files are upstream Musashi, which
files are local zimh integration glue, and how to update Musashi again later
without hand-auditing a tangled mix of upstream and local changes.

## Current Situation

The AltairZ80 M68K support currently carries Musashi-derived files under
`simulators/AltairZ80/m68k/`.  The tree also contains an `example/` copy under
that directory, so there may be more than one embedded copy of upstream Musashi
material.

The local files have accumulated zimh/SIMH integration changes, warning fixes,
line-ending normalization, whitespace cleanup, and build-system integration.
Some of those changes may be legitimate local adapter code, while others may be
obsolete once the upstream version is vendored cleanly.

## Constraints

- Do not make a large source replacement without automated tests around the
  current behavior first.
- Keep upstream Musashi files as close to upstream as practical.
- Put local integration code in separate zimh-owned files where possible.
- Avoid making broad style cleanup inside vendored files.
- Preserve license and copyright notices from upstream.
- Record the exact upstream commit or tag used for the vendor import.
- Do not hide local changes inside a single opaque import commit.

## Proposed Layout

Use a dedicated vendor location, for example:

```text
third_party/musashi/
```

Keep zimh-specific configuration and adapters outside the vendor copy, for
example:

```text
simulators/AltairZ80/m68kconf_zimh.h
simulators/AltairZ80/m68k_adapter.c
simulators/AltairZ80/m68k_adapter.h
```

The exact names can change during implementation, but the ownership split
should remain:

- `third_party/musashi/`: upstream code copied from `kstenerud/Musashi`.
- `simulators/AltairZ80/`: zimh integration, memory callbacks, simulator glue,
  build wiring, and tests.

## Work Plan

1. Inventory all Musashi-derived files currently in the repository.
   - Search for `MUSASHI`, `Karl Stenerud`, `m68kmake`, `m68kcpu`, and
     `m68kconf`.
   - Confirm whether the `example/` directory is used by any build or tests.
   - Identify whether any non-AltairZ80 simulator uses the same embedded copy.

2. Record current local behavior before changing sources.
   - Run the existing AltairZ80 integration test.
   - Add targeted tests if current coverage is not enough to catch a broken
     M68K path.
   - Prefer tests that exercise CP/M-68K boot or a small 68K instruction smoke
     program through the simulator boundary.

3. Fetch or vendor upstream Musashi.
   - Choose a specific upstream tag or commit.
   - Record the upstream URL and commit hash in a local vendor note.
   - Import the upstream files without local edits in the same commit.

4. Separate local configuration from upstream code.
   - Move zimh-specific memory access callbacks, interrupt callbacks, and
     simulator integration into zimh-owned adapter/config files.
   - Point upstream Musashi at the zimh config through its supported
     `MUSASHI_CNF` mechanism or equivalent build configuration.
   - Avoid editing upstream headers just to include zimh headers.

5. Replace the old embedded copy.
   - Update CMake to build the vendored Musashi sources.
   - Remove duplicate or obsolete embedded Musashi files from
     `simulators/AltairZ80/m68k/`.
   - Keep only local adapter/config files in the simulator directory.

6. Reapply only necessary local fixes.
   - Review old local changes one by one against upstream.
   - Keep only changes needed for zimh integration or correctness.
   - If a local correctness fix is not present upstream, document it and
     consider preparing an upstreamable patch.

7. Verify.
   - Clean build from scratch.
   - Debug-warnings build.
   - Unit tests.
   - Full integration tests.
   - A focused AltairZ80/M68K test pass.

## Commit Structure

Prefer a small series rather than one large commit:

1. Add characterization tests for current AltairZ80 M68K behavior.
2. Import upstream Musashi into `third_party/musashi/` unchanged.
3. Add zimh adapter/config files and build wiring.
4. Switch AltairZ80 to the vendored Musashi.
5. Remove obsolete embedded Musashi copies.
6. Apply any required local fixes as separate, explained commits.

## Risks

- The old embedded copy may contain local fixes not present upstream.
- The current build may depend on non-obvious include ordering or direct
  inclusion of `.c` files.
- Generated Musashi files may need a clear policy: vendor generated outputs,
  regenerate during build, or regenerate only during vendor updates.
- The `example/` tree may be unused but still valuable as upstream reference;
  decide whether vendoring upstream documentation/examples is useful or just
  noise.

## Open Questions

- Which upstream commit or tag should be the first vendored baseline?
- Should the vendor import include upstream examples and generator tools, or
  only the runtime files needed by zimh?
- Do we want a local `third_party/musashi/README.zimh.md` documenting the
  upstream commit and update procedure?
- Are there existing AltairZ80 M68K tests strong enough to catch regressions, or
  do we need a new CP/M-68K smoke test before replacement?
