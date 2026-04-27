# Replace Bundled Slirp With Upstream libslirp

## Goal

Stop building the old QEMU-derived slirp snapshot in `third_party/slirp` and
use the maintained upstream libslirp library instead.

The desired end state is:

- zimh depends on upstream libslirp when SLIRP networking is enabled;
- `third_party/slirp` and the local QEMU compatibility stubs are removed;
- zimh keeps only the adapter code needed to connect `sim_ether.c` to the
  public libslirp API; and
- the build records and reports the libslirp dependency clearly.

The upstream project is:

```text
https://gitlab.freedesktop.org/slirp/libslirp
```

## Current Situation

`third_party/slirp/README` says the bundled code was forked from QEMU 2.4.0.1
on 6-Oct-2015, specifically from QEMU commit:

```text
83c92b45140be773f0c5545dddea35a89db1ad03
```

The current tree builds that copy directly when `WITH_SLIRP` is enabled.  The
same static library also compiles `third_party/slirp_glue/sim_slirp.c` and
`third_party/slirp_glue/glib_qemu_stubs.c`, which adapt the old QEMU slirp
code to zimh.

That arrangement has several problems:

- the bundled networking stack is old and may miss years of correctness and
  security fixes;
- local warning cleanup in `third_party/slirp` would make future resyncs
  harder;
- the local QEMU compatibility headers obscure what API zimh actually needs;
  and
- the build makes slirp look like zimh-owned source rather than an external
  dependency.

## Constraints

- Do not make warning-cleanup changes inside the old bundled slirp snapshot
  unless they are required as a temporary build fix.
- Prefer a normal external dependency over another ad-hoc vendor import.
- Keep the zimh adapter small and clearly owned by zimh.
- Preserve existing user-visible SLIRP behavior where practical, including
  NAT networking, DHCP defaults, TFTP boot support, and port forwarding.
- Keep the dependency optional: builds without SLIRP support should still be
  possible when networking or libslirp is unavailable.
- Test the adapter behavior directly where possible, and add integration
  coverage for at least one simulator using SLIRP networking.

## Proposed Build Shape

Use CMake dependency discovery for libslirp instead of `add_subdirectory` on
the bundled source.

Likely detection order:

1. Try `pkg-config`/`PkgConfig` for the `slirp` package.
2. If libslirp provides a usable CMake package on a platform, support that as
   an additional discovery path.
3. If `WITH_SLIRP=ON` and libslirp is missing, fail configuration with a clear
   message naming the required package.
4. If `WITH_SLIRP=OFF`, build without SLIRP as today.

The resulting target should expose one internal dependency target to the rest
of the build, for example `zimh_slirp_dependency`, so simulator and runtime
targets do not care whether libslirp was found through pkg-config or CMake.

Package names to check during implementation:

- Homebrew: likely `libslirp`
- Debian/Ubuntu: likely `libslirp-dev`
- Fedora: likely `libslirp-devel`
- Windows: decide whether the supported path is vcpkg, MSYS2, or a manually
  installed package

## Adapter Work

Modern libslirp has a public API independent of QEMU.  The adapter should move
away from old `slirp_init()`/QEMU compatibility assumptions and use the modern
configuration/callback API, probably `slirp_new()` with `SlirpConfig` and
`SlirpCb`.

The zimh-owned adapter should be responsible for:

- parsing existing `NAT:`/SLIRP attachment options;
- translating zimh network packets into `slirp_input()`;
- delivering libslirp output packets back to `sim_ether.c`;
- integrating libslirp polling with zimh's existing select/poll path;
- handling libslirp timers through zimh-owned callbacks;
- setting up DHCP/TFTP/boot-file options equivalent to the current behavior;
  and
- adding/removing TCP and UDP host forwarding rules.

Keep this adapter outside `third_party/`, probably under `src/runtime/` or a
small `src/runtime/slirp/` subdirectory.  Once the new adapter is in place,
`third_party/slirp_glue` should disappear unless a small public header remains
useful for runtime code.

## Testing

Follow `projects/TESTING.md`: introduce tests before changing production
behavior.

Suggested coverage:

1. Add unit tests for SLIRP attachment option parsing.
   - default network;
   - custom gateway/network/netmask;
   - DHCP on/off behavior if exposed;
   - TFTP path and boot file;
   - TCP and UDP redirections;
   - malformed options and error text.

2. Add adapter tests with a fake packet sink.
   - confirm packets emitted by libslirp are passed to the simulator callback;
   - confirm zimh packets are accepted by the adapter;
   - confirm close/free paths are safe and idempotent enough for existing
     callers.

3. Add or update at least one integration test that uses SLIRP networking.
   The test should be deterministic and should not depend on external network
   access.  Prefer guest-to-host or host-forwarding behavior that can be kept
   entirely on localhost.

4. Run the normal clean build, unit tests, integration tests, and a
   debug-warnings build.

## Suggested Slices

1. Inventory the current SLIRP user-visible behavior.
   - Document accepted attach strings.
   - Document defaults for gateway, DHCP start, DNS, TFTP, and boot file.
   - Identify which simulators currently exercise SLIRP.

2. Add tests for current option parsing and behavior at the closest practical
   boundary.

3. Add CMake discovery for external libslirp while leaving the old bundled
   build path in place behind a temporary option.

4. Port `sim_slirp` to the modern libslirp API.

5. Switch the default `WITH_SLIRP` build to use external libslirp.

6. Remove `third_party/slirp`, `third_party/slirp_glue/glib_qemu_stubs.c`, and
   the QEMU compatibility headers once tests pass.

7. Remove the temporary bundled fallback option if it was introduced only to
   stage the migration.

8. Update user and maintainer documentation.

## Commit Structure

Prefer a small series:

1. Characterize current SLIRP option parsing and behavior with tests.
2. Add external libslirp discovery to CMake.
3. Port the zimh SLIRP adapter to upstream libslirp.
4. Switch builds to the external dependency.
5. Remove the bundled QEMU slirp snapshot and compatibility stubs.
6. Update documentation.

## Risks

- The modern libslirp API is not source-compatible with the 2015 QEMU snapshot.
- Timer and polling integration may be the highest-risk adapter code.
- Some current behavior may rely on old defaults that modern libslirp changed.
- Windows dependency handling may need its own path.
- There may be no existing deterministic SLIRP integration test, so a test
  seam may need to be built first.

## Open Questions

- What minimum libslirp version should zimh require?
- Which Windows dependency provider should we support first?
- Should `WITH_SLIRP=ON` be the default when libslirp is present, or should it
  remain explicitly opt-in?
- Is any current user relying on the old bundled slirp behavior in a way that
  modern libslirp no longer supports?
