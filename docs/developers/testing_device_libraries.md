# Testing Device Libraries

The simulator control package includes a built-in `TESTLIB` command for
running internal library tests from a simulator prompt. These tests are
intended for simulator developers and maintainers. They exercise shared
device libraries through real simulator devices rather than through the
unit test binary.

This facility is old, is not being actively maintained, and may be
removed in favor of unit tests in the not-too-distant future. It is
documented here because it still exists and can still be useful when
working on the legacy device-library tests.

`TESTLIB` can run every library test available in the current simulator:

```text
sim> TESTLIB
```

With no argument, `TESTLIB` runs the SCP internal tests and then runs
all available device-library tests. It can also run only the SCP tests:

```text
sim> TESTLIB SCP
```

It can run the library test for one named device:

```text
sim> TESTLIB {device}
```

For example, if a simulator names its tape device `TAPE`, this runs the
tape library tests:

```text
sim> TESTLIB TAPE
```

Use `HELP TESTLIB` inside a simulator for the built-in help text.

Run `TESTLIB` immediately after simulator startup. The command is
intended for fresh test processes and refuses to run after simulator time
has advanced.

`TESTLIB` detaches all units before it runs, so do not use it in a live
configured simulator session. Start a new simulator process in a scratch
directory when running the tests manually.

## Supported Device Libraries

`TESTLIB` dispatches to library self-tests based on the selected
device's `DEV_TYPE`:

| Device type | Library test |
|-------------|--------------|
| `DEV_CARD`  | `sim_card_test()` |
| `DEV_DISK`  | `sim_disk_test()` |
| `DEV_ETHER` | `sim_ether_test()` |
| `DEV_TAPE`  | `sim_tape_test()` |
| `DEV_MUX`   | `tmxr_sock_test()` |

Devices without one of these library types are skipped.

If a selected device is disabled, `TESTLIB` temporarily enables it for
the test and restores the disabled state afterward.

## Switches

Some library tests can produce debug output. The `-d` switch enables
that output:

```text
sim> TESTLIB -d TAPE
```

## Tape Library Tests

The tape library test creates temporary tape image files, processes them
through the regular tape attach and detach paths, verifies behavior
across supported tape image formats, and removes the generated files.

The tape test covers formats such as SIMH, E11, TPC, P7B, TAR, ANSI,
DOS11, FIXED, and AWS. In this context AWS means Architecture
Workstation tape format, not Amazon Web Services.

Because the built-in tests create and remove files in the simulator's
current working directory, run them from a scratch directory when
working manually.
