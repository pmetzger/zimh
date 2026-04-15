# vmnet-broker Design

## Goal

Add macOS host, shared, and bridged networking support without requiring
the simulator process to run as root.

The preferred direction is to use `vmnet-broker` rather than restoring
direct in-process `vmnet.framework` setup in `sim_ether.c`.

The implementation should:

- preserve the user-facing `ATTACH` syntax used by the earlier vmnet
  work:
  - `vmnet:host`
  - `vmnet:shared`
  - `vmnet:<ifname>` for bridged mode, e.g. `vmnet:en0`
- keep macOS-specific code in separate files
- minimize churn in the existing cross-platform Ethernet code
- avoid requiring Objective-C unless the broker client forces it

## Working Assumptions

This design assumes the following:

1. `vmnet-broker` provides a C client library.
2. That client library returns a serialized vmnet network object or an
   equivalent handle suitable for use with Apple's vmnet APIs.
3. Packet I/O is still done through the vmnet framework (`vmnet_read`,
   `vmnet_write`, callbacks, etc.), not through the broker itself.
4. The broker handles privilege-sensitive network acquisition, while
   SIMH remains an unprivileged consumer of the resulting vmnet network.

If assumption 2 turns out to be wrong, the design should be adjusted at
the backend file boundary rather than in the generic SIMH code.

## Why This Design

SIMH's current Ethernet layer in `src/runtime/sim_ether.c` is organized
around a small number of transport backends:

- `pcap`
- `tap`
- `vde`
- `udp`
- `nat`

These are selected in `_eth_open_port`, then handled by:

- `eth_read`
- `_eth_write`
- `_eth_close_port`
- `_eth_error`
- `eth_devices`
- `eth_attach_help`

That structure is still usable. The mistake would be putting broker,
XPC, and vmnet implementation details directly into the generic file.

Instead, add one more backend and keep its implementation in macOS-only
files.

## Chosen Architecture

### User-visible behavior

The end-user syntax should remain:

- `ATTACH XQ vmnet:host`
- `ATTACH XQ vmnet:shared`
- `ATTACH XQ vmnet:en0`

`SHOW ETHERNET` should list the same pseudo-devices.

The user should not have to learn a separate `vmnet-broker:` syntax
unless that proves necessary during implementation.

### Internal model

Add a new Ethernet backend:

- `ETH_API_VMNET`

This backend is only compiled on macOS when vmnet-broker support is
enabled.

The generic Ethernet layer remains responsible for:

- parsing attach strings
- packet filtering and loopback logic
- read/write queueing
- device bookkeeping
- user help and error propagation

The macOS backend becomes responsible for:

- parsing vmnet mode details after the `vmnet:` prefix
- talking to the broker client library
- constructing the vmnet interface
- starting and stopping the vmnet interface
- reading packets from vmnet
- writing packets to vmnet
- reporting vmnet-specific errors in textual form

## File Layout

Add these files:

- `src/runtime/sim_ether_vmnet.h`
- `src/runtime/sim_ether_vmnet.c`

Optional, only if the broker client or Apple APIs require Objective-C:

- `src/runtime/sim_ether_vmnet.m`

The preferred first attempt is pure C.

The generic file `src/runtime/sim_ether.c` should only gain:

- one new backend enum value
- one more branch in attach parsing for `vmnet:`
- small calls into the vmnet backend
- `SHOW ETHERNET` and attach-help text entries
- read/write/close/error switch entries for the new backend

No broker logic should live in `sim_ether.c`.

## Backend Boundary

Define a small private interface in `sim_ether_vmnet.h`:

```c
typedef struct sim_vmnet sim_vmnet_t;

t_stat sim_vmnet_open(const char *name,
                      sim_vmnet_t **out,
                      char *display_name,
                      size_t display_name_size,
                      char *errbuf,
                      size_t errbuf_size,
                      DEVICE *dptr,
                      uint32 dbit);

t_stat sim_vmnet_close(sim_vmnet_t *vmnet);

int sim_vmnet_read(sim_vmnet_t *vmnet,
                   uint8 *buf,
                   size_t buf_size,
                   size_t *out_len);

int sim_vmnet_write(sim_vmnet_t *vmnet,
                    const uint8 *buf,
                    size_t len);

const char *sim_vmnet_error(sim_vmnet_t *vmnet);

int sim_vmnet_get_select_fd(sim_vmnet_t *vmnet);
```

Notes:

- `display_name` is what gets reported as the opened OS device.
- `sim_vmnet_get_select_fd` may return `-1` if the backend is callback or
  queue driven rather than `select`-driven.
- The `sim_vmnet_t` structure is opaque to the generic code.

## Internal vmnet State

`sim_vmnet_t` should hold at least:

- the requested mode:
  - host
  - shared
  - bridged
- bridged interface name when applicable
- broker client handle or broker-returned serialization object
- vmnet network object
- vmnet interface handle
- any dispatch queue / callback context
- a receive queue for packets arriving asynchronously
- last vmnet status / last broker status
- textual mode name for diagnostics

The receive queue should be internal to the vmnet backend, not shared
with the higher-level `ETH_DEV` queue.

## Packet Delivery Strategy

This is the most important design decision.

The vmnet backend should manage packet ingress asynchronously and present
it to SIMH as pull-based reads.

Recommended approach:

1. On open, create a private vmnet receive queue in the backend.
2. Register the vmnet event callback required by the Apple API.
3. When packets are available, have the backend callback read them from
   vmnet and push them into the backend queue.
4. `sim_vmnet_read()` then pops one packet from that backend queue.

This is preferable to trying to expose vmnet internals directly to
`sim_ether.c`.

It also avoids forcing the generic reader thread to understand vmnet's
callback model.

### Why not require a selectable file descriptor?

The current reader-thread logic assumes:

- `pcap` either dispatches directly or can be selected
- `tap`, `vde`, and `udp` expose descriptors
- `nat` has its own helper path

vmnet may not expose a normal selectable descriptor. Requiring one would
contort the design unnecessarily.

Instead, the generic layer can treat vmnet like this:

- with `USE_READER_THREAD`, the reader thread polls
  `sim_vmnet_read()` at a short interval when `ETH_API_VMNET` is active
- without `USE_READER_THREAD`, `eth_read()` calls `sim_vmnet_read()`
  directly

If later investigation shows a clean waitable event can be surfaced, that
can be added without changing the public backend boundary.

## Attach Parsing

`sim_ether.c` should continue to recognize:

- `vmnet:host`
- `vmnet:shared`
- `vmnet:<ifname>`

Parsing rules:

- `host` means host-only mode
- `shared` means NAT/shared mode
- anything else after `vmnet:` is treated as a bridged interface name

The backend should validate the bridged interface name and return a clear
error.

The parser should reject empty `vmnet:` with a usage error.

## `eth_devices()` and Help Text

On supported macOS builds, `eth_devices()` should append:

- `vmnet:shared`
- `vmnet:host`
- one bridged example such as `vmnet:en0`

The description strings should make it explicit that the implementation
uses broker-backed vmnet support.

`eth_attach_help()` should document the three accepted forms and should
note that the helper or broker service must already be installed and
available on the host.

## Build Integration

### CMake

Add a feature option and detection logic:

- `WITH_VMNET_BROKER`

Enable it only when all of the following are true:

- host is macOS
- broker client headers are found
- broker client library is found
- vmnet framework is available

If pure C is sufficient, keep the project as C-only.

If Objective-C is required for the backend:

- enable Objective-C only for this backend target
- do not broaden the whole project language set unless necessary

The vmnet backend should be linked only into builds that need Ethernet
support on macOS.

### Makefile

Add the same feature detection and link flags to the legacy Makefile.

The Makefile and CMake must expose the same attach syntax and feature
summary.

## Error Handling

The backend should translate broker and vmnet statuses into readable
errors before they reach users.

Requirements:

- preserve the raw status code where possible
- include the mode in the error message
- distinguish broker acquisition failures from vmnet interface failures

Examples:

- `Eth: vmnet broker failed for mode shared: ...`
- `Eth: vmnet interface start failed for en0: ...`

Do not collapse everything into a generic "open error".

## Testing Plan

### First milestone

Implement enough for this manual test path:

- build on macOS
- `show ethernet` lists vmnet modes
- `attach xq vmnet:host`
- `attach xq vmnet:shared`
- `attach xq vmnet:en0`

Use the existing manual script as a template:

- `tests/manual/test-pdp11-vmnet.sh`

It should be updated later to document the broker-backed implementation.

### Regression goals

After the first milestone:

- verify the normal UDP backend still works
- verify `pcap` builds still work
- verify no non-macOS build gains new dependencies
- verify the legacy Makefile build still works on macOS

### Longer-term tests

If the backend gets extracted cleanly enough, add a host-side unit-test
target for its parsing and error translation logic. Full packet I/O may
remain manual initially.

## Implementation Phases

### Phase 1: Skeleton

- add `ETH_API_VMNET`
- add new backend files
- add build detection
- wire `vmnet:` parsing to the backend open call
- make `SHOW ETHERNET` advertise the feature

No packet I/O yet; attach may still fail with "not implemented".

### Phase 2: Open and close path

- acquire broker-backed network
- create/start vmnet interface
- implement close
- return meaningful errors

At the end of this phase, attach and detach should succeed.

### Phase 3: Packet I/O

- implement receive queue
- implement vmnet callback/read path
- implement write path
- run PDP-11 `XQ` manual tests

At the end of this phase, basic Ethernet traffic should work.

### Phase 4: Cleanup

- improve `SHOW ETHERNET` output
- refine diagnostics
- update build docs
- decide whether to mention the broker explicitly in user-facing help

## Open Questions

These need to be answered during implementation:

1. What exact object type does the broker C client return?
2. Which vmnet API constructs the usable network object from it?
3. Does vmnet expose a waitable event, or should the backend own the
   receive pump entirely?
4. Can the backend remain pure C, or does some Apple-side step require
   Objective-C?
5. What installation model should SIMH document for the broker service?

None of these questions should require redesign of the generic backend
boundary.

## Recommendation

Proceed with a broker-backed macOS vmnet backend implemented in separate
files and integrated into `sim_ether.c` as one additional transport.

Do not reintroduce the old direct vmnet setup into the generic Ethernet
file.
