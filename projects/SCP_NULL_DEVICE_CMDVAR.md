# SCP Null Device Command Variable

## Problem

SCP has an internal portable null-device name, `NULL_DEVICE`, which maps to
`NUL:` on Windows and `/dev/null` elsewhere. The command processor also opens
`stdnul` from that name for internal bit-bucket use.

Command files do not currently have an equivalent built-in variable. Scripts
that want to attach a simulated output device to a discard sink are therefore
tempted to spell a host-specific name such as `nul`, `NUL:`, or `/dev/null`.
That is not portable. In particular, `nul` is just an ordinary relative file
name on POSIX hosts.

## Desired Fix

Expose the portable null-device path through command variable expansion, for
example as `%SIM_NULL_DEVICE%`.

That would let scripts write:

```
att cdp1 -q "%SIM_NULL_DEVICE%"
```

without branching on `%SIM_OSTYPE%` or creating temporary throwaway files.

## Testing

Add unit coverage for command variable expansion confirming that the new
variable expands to the same platform-specific spelling as `NULL_DEVICE`.
Add at least one integration-style command-file test that attaches an output
unit to the variable and verifies that no ordinary file named `nul` is created
on POSIX hosts.
