# TMXR Modernization

The current terminal multiplexer is centered on Telnet and serial-style
line handling. Telnet support is still useful for compatibility, but it
is now an obsolete default for new deployments.

This note tracks adding a more modern alternative alongside the
existing TMXR support.

Goals:

- keep current TMXR/Telnet behavior working for existing users
- add a modern remote-console transport that does not depend on Telnet
- keep simulator-facing line/console abstractions stable where possible
- move transport-specific logic out of simulator code and into shared
  runtime support

Candidate directions:

- WebSocket-based console transport
- SSH-backed console access

The first practical step should be one of the secure or higher-level
options above, rather than another plain-text socket mode.

Likely work items:

- audit where TMXR assumes Telnet semantics instead of generic stream I/O
- separate transport-neutral line buffering from Telnet protocol handling
- define configuration syntax for selecting Telnet vs modern transport
- add unit and integration coverage for the new transport mode

Non-goals for the first pass:

- deleting Telnet support
- redesigning simulator console semantics
- introducing browser UI requirements
