# `src/compat`

This directory is for small compatibility functions that provide
standard APIs missing from some supported host platforms.

Typical contents include implementations imported or adapted from
well-established upstream sources, such as OpenBSD libc routines, when
the host platform does not provide them itself.

Examples of the intended scope:

- `strlcpy`
- `strlcat`
- `localtime_r`
- `gmtime_r`
- other small libc-style compatibility routines

Rules for code placed here:

- keep this directory narrowly focused on compatibility shims
- prefer well-known, established implementations over ad hoc rewrites
- build each shim only on the platforms that actually need it
- keep higher-level runtime or simulator logic out of this directory

This directory is not for general utilities, runtime services, or new
cross-platform abstractions. Those belong elsewhere in `src/`.
