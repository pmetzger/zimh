# Network Testing

## Goal

Build automated tests that exercise simulator networking realistically.

The high-level tests should boot real guest operating systems and use the
network through emulated NICs.  The lower-level tests should mock the host
sockets API so network backends can be tested more deeply without relying on
the real host network.

## Guest OS Images

Create custom NetBSD/VAX disk images for realistic backend testing.  NetBSD is
a good first guest because it supports the VAX machines we emulate and has
normal IP networking tools in the base system.

Use separate image variants or boot-time scripts for the backends we support:

- SLIRP/NAT;
- PCAP on a controlled host interface;
- TAP on a controlled host interface;
- VDE, when the host dependency is available; and
- VMNET on macOS, when applicable.

Each image should contain:

- a minimal bootable system;
- one configured Ethernet interface;
- backend-specific IPv4 and IPv6 configuration;
- backend-specific resolver configuration;
- shell scripts that run the network test plan and produce machine-readable
  results; and
- only deterministic local services and fixtures needed by the test.

The image should be produced by a documented script, not hand-built state.
Keep the generated image out of git unless we later decide to store binary
test artifacts somewhere explicit.

For SLIRP, the initial configuration should use the current defaults:

- IPv4 address `10.0.2.15/24`;
- IPv4 gateway `10.0.2.2`;
- IPv4 DNS proxy `10.0.2.3`;
- IPv6 address `fd00::15/64`;
- IPv6 gateway `fd00::2`; and
- IPv6 DNS proxy `fd00::3`.

## Host Harness

Add a host-side integration harness that:

- boots the VAX emulator with the selected network backend attached;
- attaches the prepared NetBSD disk image;
- starts any controlled host-side network services or virtual interfaces;
- captures console output;
- waits for a clear pass/fail marker from the guest script;
- enforces a timeout; and
- saves emulator and guest logs on failure.

The default tests should not require internet access.  Prefer host-local
endpoints, controlled TAP/PCAP/VDE/VMNET networks, SLIRP virtual services, and
emulator-visible forwarded ports.

## Guest Test Plan

The guest script should exercise at least:

- interface bring-up and route installation;
- ARP resolution for IPv4 peers and gateways;
- IPv4 ICMP echo;
- IPv6 neighbor discovery;
- IPv6 ICMP echo;
- DNS lookups through the configured resolver;
- TCP client connections;
- UDP datagram send/receive;
- inbound TCP host forwarding to a guest service;
- inbound UDP host forwarding to a guest service, if supported cleanly;
- multiple sequential connects to catch lifecycle leaks;
- a modest concurrent TCP workload;
- larger TCP transfers to catch buffering and close handling problems; and
- clean guest shutdown after the test finishes.

If a case needs external routing to be meaningful, split it into a separate
optional test.  The default CI-safe suite should remain host-local.

## Host Services

For deterministic client tests, the harness can start local host services
before booting the guest:

- an HTTP or raw TCP echo service on localhost;
- a UDP echo service on localhost;
- a DNS fixture when resolver behavior needs deterministic answers; and
- listeners for guest-to-host and host-to-guest forwarding checks.

The harness should own these services and tear them down even after failures.

## Socket-Mocked Unit Tests

Add more realistic unit tests by mocking the host sockets API underneath the
network backends.  The goal is to test backend behavior that currently needs
real sockets, real file descriptors, or real host network state.

The socket mock should let tests script and inspect calls such as:

- `socket`;
- `bind`;
- `listen`;
- `accept`;
- `connect`;
- `send`, `sendto`, `recv`, and `recvfrom`;
- `select`, `poll`, or platform equivalents;
- `getsockopt` and `setsockopt`; and
- `close` or `closesocket`.

This is especially useful for error paths, retry behavior, partial I/O,
readiness notifications, and backend cleanup.  It can also make libslirp
outbound behavior testable without external network access.

The implementation will be platform-sensitive.  Possible techniques include
ELF link wrapping, `LD_PRELOAD`, macOS interposition, and Windows import
library substitution.  Keep this work isolated behind a test-only abstraction
so production code does not acquire test-only socket shims.

## Done Criteria

- NetBSD/VAX images can be rebuilt from scripts.
- The VAX emulator can boot each image and run the guest network script
  unattended.
- The guest tests cover IPv4, IPv6, DNS, TCP, UDP, and backend-specific
  forwarding behavior at the OS level.
- Socket-mocked unit tests cover backend socket behavior without real network
  access.
- The default suite does not depend on external network access.
- Failures leave enough logs to identify whether the emulator, SLIRP, the
  selected backend, the guest OS, or the harness failed.
