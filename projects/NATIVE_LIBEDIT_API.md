# Switch SCP to the native libedit API

## Problem

`scp.c` currently uses the readline compatibility API exposed by libedit.
Different platforms install those compatibility headers in different
locations, such as `editline/readline.h` or `readline/readline.h` plus
`readline/history.h`. This forces the build to carry compatibility-header
detection that is unrelated to libedit's native API.

## Proposed change

Move SCP command-line editing to libedit's native `histedit.h` API
directly. The implementation should preserve the existing no-editline
fallback path.

Once SCP no longer calls `readline()` and `add_history()` directly, remove
the build checks for readline compatibility header variants:

- `HAVE_EDITLINE_READLINE_H`
- `HAVE_READLINE_READLINE_H`
- `HAVE_READLINE_HISTORY_H`
- `HAVE_EDITLINE_HISTORY_H`

At that point `FindEDITLINE.cmake` can go back to detecting `histedit.h`
and the editline library as the public dependency contract.

## Testing

Add automated coverage for command input with and without editline enabled.
The tests should cover prompt handling, empty input, history insertion, and
EOF behavior.
