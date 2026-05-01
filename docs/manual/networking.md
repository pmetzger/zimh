# SIMH Networking Guide

**Copyright Notice**

The SIMH source code and documentation is made available under a
X11-style open source license; the precise terms are available at:

<https://github.com/open-simh/simh/blob/master/LICENSE.txt>

# Introduction

SIMH supports several ways to connect simulated Ethernet devices to the
outside world.  This document describes the current networking options
from a user's point of view.

Not every simulator has an Ethernet device, and the device name varies
by simulator.  Common examples are `XQ` and `XU` on PDP-11 and VAX
systems.  Use the simulator's own `HELP` command to see the device name
and its attach help.

# Basic Workflow

The common pattern is:

1. Build a simulator with networking support.
2. Start the simulator.
3. Use `SHOW ETHERNET` to list the available host interfaces and
   pseudo-network backends.
4. Attach the simulated Ethernet device to the transport you want.
5. Configure the guest operating system for that network.

A minimal session looks like this:

```text
sim> SHOW ETHERNET
sim> ATTACH XQ en0
```

On systems where the exact host interface name is awkward, `SHOW
ETHERNET` will also display short aliases such as `eth0`, `eth1`, and
so on.  Those aliases may be used in `ATTACH` commands as well.

# Choosing a Connection Type

SIMH currently supports several different attachment styles.

- `ATTACH XQ en0`
  Full LAN access on a real host interface.  Use this when the guest
  must participate directly on the local Ethernet, including non-IP
  protocols.  Often requires elevated privilege on Unix-like systems.

- `ATTACH XQ nat:`
  The easiest way to get outbound TCP/IP connectivity.  Good for simple
  Internet access.  Does not carry DECnet, LAT, or other non-IP
  traffic.

- `ATTACH XQ nat:tcp=2323:10.0.2.15:23`
  NAT with selected inbound services.  Use this when the guest should
  stay behind NAT but a few guest TCP services should be reachable from
  the host.

- `ATTACH XQ tap:tap0`
  Host-controlled bridging or routing.  Use this when you want explicit
  control over how the guest connects to the host network.  The TAP
  device and any bridge or route must already exist before SIMH starts.

- `ATTACH XQ vde:/path/to/switch`
  Integration with a VDE switch.  Useful on systems that already use
  VDE.

- `ATTACH XQ udp:10000:host:10001`
  Point-to-point bridging or custom adapters.  Useful for
  simulator-to-simulator links and for external bridge/helper programs.

If you only need ordinary TCP/IP, start with `nat:`.  If you need the
guest to appear directly on a real LAN, use a real interface or a
preconfigured TAP setup.

# Inspecting Available Interfaces

Use:

```text
sim> SHOW ETHERNET
```

This lists:

- real host interfaces that SIMH can see through `libpcap` or the
  platform packet-capture layer
- pseudo-devices such as `tap:tapN`, `vde:...`, `nat:...`, and
  `udp:...`

`SHOW ETHERNET` is the first command to run when networking does not
behave as expected.  It tells you what SIMH thinks is available on the
current host.

# The Main Attachment Modes

## Direct Host Interface Access

Attaching to a real host interface gives the guest the most realistic
behavior:

```text
sim> ATTACH XQ en0
```

Use this when the guest must speak protocols other than TCP/IP, or when
you want it to appear directly on the physical LAN.

This is usually the most privilege-sensitive mode.  On Unix-like
systems, SIMH may need enough privilege to place the interface in
promiscuous mode and transmit packets through the packet-capture layer.

## NAT

NAT is the simplest mode for many users:

```text
sim> ATTACH XQ nat:
```

This gives the guest a private virtual network with outbound access
through the host.  It is intended for IP traffic.  The `nat:` backend is
available only when the simulator was built with SLIRP support; builds
using the external SLIRP backend require `libslirp`.

Typical built-in addresses are:

- guest static addresses: `10.0.2.4` through `10.0.2.14`
- DHCP guest address: `10.0.2.15`
- gateway: `10.0.2.2`
- DNS server: `10.0.2.3`
- netmask: `255.255.255.0`
- IPv6 prefix: `fd00::/64`
- IPv6 gateway: `fd00::2`
- IPv6 DNS server: `fd00::3`

IPv6 is enabled by default when `nat:` is backed by external libslirp.
The IPv6 options are:

- `ipv6`: enable IPv6.
- `noipv6`: disable IPv6.
- `ipv6prefix=prefix/prefixlen`: set the guest IPv6 prefix.
- `ipv6gateway=address`: set the guest-visible IPv6 gateway address.
- `ipv6dns=address`: set the guest-visible IPv6 DNS proxy address.
- `ipv6nameserver=address`: synonym for `ipv6dns`.

For example, use `noipv6` to disable IPv6:

```text
sim> ATTACH XQ nat:noipv6
```

To choose a different guest IPv6 prefix or virtual IPv6 service
addresses, use:

```text
sim> ATTACH XQ nat:ipv6prefix=fd42:1234::/64
sim> ATTACH XQ nat:ipv6gateway=fd42:1234::2,ipv6dns=fd42:1234::3
```

If you want the host to reach a guest service while still using NAT,
forward a host TCP port:

```text
sim> ATTACH XQ nat:tcp=2323:10.0.2.15:23
```

That example forwards host port `2323` to guest port `23`.

NAT is a good default choice when:

- the guest only needs TCP/IP
- you want the lowest-friction setup
- you do not need the guest to appear directly on the LAN

Do not use NAT if you need Ethernet-level protocols such as DECnet,
LAT, or other non-IP traffic.

## TAP

TAP mode uses a host-created virtual Ethernet interface:

```text
sim> ATTACH XQ tap:tap0
```

SIMH does not create the TAP device or configure any bridge or route
around it.  That surrounding host networking must already exist before
the simulator starts.

Use TAP when:

- you want explicit host-side control over bridging or routing
- you want host-to-guest communication without attaching directly to a
  physical interface
- your platform already has a known TAP workflow

## VDE

VDE mode connects the guest to a Virtual Distributed Ethernet switch:

```text
sim> ATTACH XQ vde:/tmp/switch
```

VDE, or Virtual Distributed Ethernet, is a user-space virtual
switching system for connecting virtual machines and emulators into a
software-defined Ethernet segment.  Project page:
<https://sourceforge.net/projects/vde/>.

This is useful mainly on hosts that already use VDE.  It is less common
than direct interface access, NAT, or TAP, but it can be a good fit for
lab-style virtual networks.  It is usually more flexible than direct
host-interface attachment, but it also tends to have higher overhead.

## UDP

UDP mode is a simple packet bridge between SIMH and another endpoint:

```text
sim> ATTACH XQ udp:10000:127.0.0.1:10001
```

This is especially useful for:

- simulator-to-simulator links
- debugging
- external helper or bridge processes
- experimental networking backends

UDP mode is not a replacement for direct Ethernet capture on a real LAN,
but it is a practical integration point.

# Platform Notes

## Windows

Windows Ethernet support relies on `Npcap` or `WinPcap`, with `Npcap`
being the current choice.  Once the packet-capture driver is installed
and configured correctly, normal users can often run the simulator
without additional privileges.

Direct host-interface attachment on Windows can often communicate with
the host over that same interface, which is convenient for simple local
testing.

## Linux and Other Unix-like Systems

Direct host-interface access normally depends on `libpcap`.

On these systems, attaching a simulated Ethernet device to a real host
interface often requires elevated privilege or host configuration that
grants the needed packet-capture access.  TAP setups can reduce the need
to run the simulator itself with elevated privilege, but the TAP device
and any related bridge or route still have to be created outside SIMH.

On many Unix-like hosts, direct attachment to a real interface does not
behave like a simple host-and-guest loopback path.  The guest may be
able to reach the LAN while the host still cannot talk to the guest over
that same interface in the way users expect.

## macOS

Current macOS support follows the same general `libpcap` model as other
Unix-like systems.  Direct attachment to a host interface may therefore
require elevated privilege.

The likely future direction on macOS is broker-based networking built on
Apple's modern virtualization and vmnet facilities.  The goal of that
work is to make host, shared, and bridged networking available without
requiring the simulator process itself to run as root.  Until that work
lands, assume that direct host-interface attachment on macOS is still a
privileged operation.

For host-to-guest communication on macOS today, `nat:`, TAP-based
setups, and helper or bridge arrangements are usually more predictable
than direct attachment to a real interface.

# Troubleshooting

## `SHOW ETHERNET` Shows No Usable Devices

Possible causes include:

- the simulator was built without networking support
- the required host libraries are missing
- SLIRP support, or `libslirp` for an external SLIRP build, is missing
- the host packet-capture layer is not installed or not usable
- the current account lacks the required privilege

Start by checking the build instructions in [BUILDING.md](../../BUILDING.md)
and then rerun `SHOW ETHERNET`.

## The Guest Can Reach the LAN but Not the Host

This is a common surprise with direct host-interface attachment on
Unix-like systems.  Depending on the host platform and packet-capture
path, packets injected by the simulator may not be reflected back up the
host's own network stack in the way users expect.

If host-to-guest communication matters, consider:

- NAT with explicit port forwarding
- a TAP-based setup
- a UDP bridge/helper arrangement

This difference is usually much less surprising on Windows, where
direct-interface mode more often permits host and guest communication on
the same interface.

## `ATTACH` Fails with a Permission Error

That usually means the chosen transport needs more host privilege than
the current process has.  Try `nat:` first if you only need TCP/IP.  If
you need full LAN participation, fix the host-side privilege or packet
capture setup rather than assuming the simulator configuration is wrong.

## I Only Need Telnet, FTP, or Similar TCP/IP Services

Use `nat:` and forward only the ports you need.  That keeps the setup
simple and avoids many host-interface privilege problems.

# Getting More Detail

For the exact attach syntax and device-specific notes for a given
simulator, use the simulator's built-in help:

```text
sim> HELP XQ ATTACH
sim> HELP XU ATTACH
```

Those device help entries are the authoritative source for the
simulator-specific command syntax.  `SHOW ETHERNET` is the authoritative
source for what the current host build can actually use.
