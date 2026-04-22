# TMXR Subsystem Testing

Goal: add a middle testing layer for `src/runtime/sim_tmxr.c` between the
existing low-level unit tests and the existing full-machine integration tests.

Why this is needed:

- `sim_tmxr.c` is no longer a small helper. It is a real communications
  subsystem with multiple transport and policy modes:
  - Telnet stream handling
  - raw stream handling
  - outbound connect and reconnect behavior
  - mux and line listener behavior
  - serial-port integration
  - packet and datagram transport
  - modem-control behavior
  - status/help/reporting
- the current unit tests are valuable, but many of them are intentionally
  narrow:
  - helper-level
  - parser-level
  - single-path state transitions
  - seam-driven branch coverage
- the full integration tests are also valuable, but they prove TMXR only
  indirectly through complete simulated machines and SCP command flows
- there is still a coverage and confidence gap:
  - "does TMXR itself still behave correctly as a communications subsystem
    across realistic multi-step scenarios?"

So the missing layer is not full-system integration testing. It is better
described as:

- subsystem tests
- component tests
- black-box module tests
- contract tests

The name I would use here is:

- TMXR subsystem tests

because that most accurately describes the intent.

What this layer should do:

- drive the real public TMXR entry points
- use deterministic fake host-I/O seams underneath where needed
- avoid depending on a whole simulated machine or monitor command flow
- validate multi-step scenarios rather than isolated helpers
- assert externally visible TMXR behavior, not just internal counters or
  implementation details

What this layer should not do:

- it should not duplicate the tiny helper unit tests
- it should not turn into another full-system simulator regression suite
- it should not depend on a real host network or real serial devices
- it should not overfit to line-by-line implementation details that we want
  freedom to refactor

Suggested structure:

- keep the existing TMXR unit tests for:
  - pure helper logic
  - parse/validate code
  - seam-driven branch coverage
- add a new TMXR subsystem test target for:
  - scenario-driven tests using real TMXR public APIs
  - deterministic fake host behavior
  - state transitions over several calls and polling cycles
- continue to keep full machine-level integration tests separately

So the long-term testing stack becomes:

1. Unit tests
   For helpers, parsers, narrow state transitions, and edge-condition coverage.

2. TMXR subsystem tests
   For realistic TMXR scenarios driven through the public API without needing
   a complete simulated machine.

3. Full integration tests
   For end-to-end behavior in real simulated systems and SCP command flows.

Why the subsystem layer matters specifically for TMXR:

- TMXR now spans responsibilities that are broader than the original
  "terminal multiplexer" idea
- the file is being decomposed and cleaned up
- refactoring confidence will be much better if we can say:
  - the helpers still pass unit tests
  - the real TMXR API still passes subsystem tests
  - complete machines still pass integration tests
- without this middle layer, we are forced to choose between:
  - low-level tests that are too local
  - end-to-end tests that are too indirect and too expensive to diagnose

The kinds of TMXR scenarios this layer should cover:

- attach/open a mux, accept a connection, exchange data, detach
- outbound connect succeeds after one or more polls
- outbound connect fails, retries, then succeeds
- listener acceptance on mux-wide and line-specific listeners
- retained peer-address state after listener accept
- modem-control-driven connect/disconnect behavior
- reconnect/reset behavior across several polling cycles
- packet send/receive preserving message boundaries
- datagram behavior staying packet-oriented rather than stream-oriented
- Telnet mode and non-Telnet mode behavior through public APIs
- line buffering, transmit drain, and receive queue transitions under realistic
  call sequences
- logging and status/reporting behavior remaining coherent as state changes
- serial-pending and serial-connected transitions through the real public API

Subsystem scenarios that are especially important because unit tests are too
low-level to give enough confidence by themselves:

- `tmxr_open_master()` and the attach/open/restore contract
  - option parsing may be unit tested
  - but the overall attach/open state transition also needs scenario tests
- `tmxr_poll_conn()`
  - many branches exist only in the context of repeated polling
  - subsystem tests should model multi-poll connection lifecycles
- `tmxr_poll_rx()` and `tmxr_poll_tx()`
  - these are where buffering, packet handling, and enable/disable behavior
    interact over time
- listener acceptance paths
  - these often need realistic line/mux state across multiple calls
- packet/datagram paths
  - these need tests that prove message boundaries are preserved at the
    subsystem surface, not just in helper calls

Likely implementation approach:

- create a dedicated test fixture layer for TMXR subsystem tests
- reuse the TMXR-local host-I/O seams already introduced for unit testing
- prefer deterministic scripted fake-host behavior:
  - scripted connect result sequence
  - scripted accept sequence
  - scripted reads/writes
  - scripted peer-name lookups
  - scripted serial state transitions
- add helper functions for:
  - building realistic mux/line fixtures
  - attaching/opening through public APIs
  - advancing polling cycles
  - asserting externally visible state

Design guidance for those tests:

- prefer asserting public behavior and stable externally visible state
- avoid over-asserting incidental temporary fields unless they are part of the
  contract
- write scenarios in temporal order so the tests read like behavior, not like
  implementation archaeology
- if a subsystem test feels too hard to write, treat that as architectural
  feedback and consider whether another pure helper extraction or another very
  narrow seam is warranted

Architectural benefit:

- subsystem tests will make the planned TMXR decomposition safer
- they will help distinguish:
  - logic that should move into pure helpers
  - logic that should remain in host-I/O wrappers
  - logic that should stay visible only through public APIs
- they will also help us avoid turning every difficult path into a purely
  seam-driven unit test, which can otherwise drift toward testing mocks more
  than behavior

Backlog ideas for initial subsystem-test scenarios:

- single-line mux attach, listener accept, character exchange, detach
- outbound line connect with retry and eventual success
- datagram line send/receive preserving packet boundaries
- packet-oriented line send/receive with queued packet transitions
- line-specific listener accept retaining peer identity correctly
- serial-pending line becoming connected after the expected wait/poll path
- reconnect after reset on a configured outbound destination
- logging enabled through a connection lifecycle, then disabled cleanly

Rule:

- If new TMXR subsystem-level scenarios become obvious while expanding unit
  coverage or decomposing `sim_tmxr.c`, add them here instead of relying on
  memory.
