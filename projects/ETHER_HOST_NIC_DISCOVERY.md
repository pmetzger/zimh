# Replace sim_ether shell-based host NIC discovery

## Problem

`src/runtime/sim_ether.c` currently discovers some host NIC information by
building shell commands and parsing command output.  This is fragile:

- it depends on tools such as `ip`, `ifconfig`, and `grep`;
- it depends on command output formats that vary across hosts and releases;
- it is awkward to quote safely and correctly;
- it is hard to unit test without running host-specific commands; and
- warning-cleanup changes around these command strings are more invasive than
  they should be.

The current warning-cleanup work should not try to solve this.  It should only
preserve existing behavior while making the compiler happy.  The longer-term
goal is to replace the shell probing with host APIs and make the logic
testable.

## Goals

- Replace shell-command probing with direct host APIs where practical.
- Preserve existing behavior for supported networking backends.
- Keep any unavoidable shell fallback isolated and heavily tested.
- Separate host interface enumeration from policy decisions about which MAC
  address to use.
- Make the core selection and formatting logic unit-testable without requiring
  real network devices.

## Candidate Platform APIs

Linux:

- Prefer netlink for interface enumeration and link-layer addresses if it is
  practical within the existing portability layer.
- Consider `getifaddrs()` plus targeted `ioctl()` calls if that gives adequate
  coverage with less code.

macOS and BSD:

- Use `getifaddrs()` for interface enumeration.
- Use link-layer address information from `sockaddr_dl` where available.

Windows:

- Use the IP Helper API, likely `GetAdaptersAddresses`, to enumerate adapters
  and physical addresses.

Fallbacks:

- If a direct API is unavailable on a supported host, keep any command-based
  fallback in one small module with explicit comments explaining why it remains.
- Do not intermix fallback command construction with the normal discovery path.

## Proposed Shape

Create a small internal abstraction for host NIC discovery, probably in a new
runtime source file once the shape is clear:

- one function enumerates host interfaces into a simple internal structure;
- one function finds an interface by simulator/network device name;
- one function returns the physical MAC address if known; and
- `sim_ether.c` consumes that abstraction rather than constructing commands.

The abstraction should avoid exposing simulator internals unless necessary.

## Testing

Unit tests should not depend on real host network devices.

- Test command-free selection logic with synthetic interface lists.
- Test edge cases: missing interface, interface without MAC address, duplicate
  names, empty names, long names, and unusual but valid MAC bytes.
- If a shell fallback remains, unit test command construction and parsing with
  fixed sample output.
- Add host-conditional tests only for thin API wrappers, and keep them skipped
  or non-fatal when the host cannot provide deterministic data.

## Non-Goals

- Do not change packet capture, TAP, VDE, NAT, or UDP backend behavior as part
  of the first cleanup slice.
- Do not remove legacy command probing until replacement behavior is verified
  on Linux, macOS/BSD, and Windows.
- Do not bundle this into warning cleanup commits.

## Suggested Slices

1. Document the current shell probing behavior and identify each caller's
   required data.
2. Introduce a small internal data structure for enumerated host interfaces.
3. Add pure unit tests for host-interface selection logic.
4. Implement macOS/BSD and Linux enumeration behind the abstraction.
5. Implement Windows enumeration behind the same abstraction.
6. Keep or remove shell fallback based on coverage gaps found during porting.
7. Delete obsolete command construction and parsing once direct APIs are proven.
